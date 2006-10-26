//+----------------------------------------------------------------------------+
//| Description:  Magic Set Editor - Program to make Magic (tm) cards          |
//| Copyright:    (C) 2001 - 2006 Twan van Laarhoven                           |
//| License:      GNU General Public License 2 or later (see file COPYING)     |
//+----------------------------------------------------------------------------+

// ----------------------------------------------------------------------------- : Includes

#include <util/io/package_manager.hpp>
#include <util/error.hpp>
#include <data/game.hpp>

// ----------------------------------------------------------------------------- : IncludePackage

/// A package that just contains a bunch of files that are used from other packages
class IncludePackage : public Packaged {
  protected:
	String typeName() const;
	DECLARE_REFLECTION();
};

String IncludePackage::typeName() const { return _("include"); }

IMPLEMENT_REFLECTION(IncludePackage) {
	if (tag.reading()) {
		// ingore
		String full_name, version;
		REFLECT(full_name);
		REFLECT(version);
	}
}

// ----------------------------------------------------------------------------- : PackageManager

String program_dir() {
	return wxGetCwd(); //TODO
}

PackageManager packages;


PackageManager::PackageManager() {
	// determine data directory
	data_directory = program_dir();
	// check if this is the actual data directory, especially during debugging,
	// the data may be higher up:
	//  exe path  = mse/build/debug/mse.exe
	//  data path = mse/data
	while (!wxDirExists(data_directory + _("/data"))) {
		String d = data_directory;
		data_directory = wxPathOnly(data_directory);
		if (d == data_directory) {
			// we are at the root -> 'data' not found anywhere in the path -> fatal error
			throw Error(_("The MSE data files can not be found, there should be a directory called 'data' with these files"));
		}
	}
	data_directory += _("/data");
}

PackagedP PackageManager::openAny(const String& name) {
	wxFileName fn(data_directory + _("/") + name);
	fn.Normalize();
	String filename = fn.GetFullPath();
	// Is this package already loaded?
	PackagedP& p = loaded_packages[filename];
	if (p) {
		return p;
	} else {
		// load with the right type, based on extension
		if      (fn.GetExt() == _("mse-game"))         p = new_shared<Game>();
//		else if (fn.GetExt() == _("mse-style"))        p = new_shared<CardStyle>();
//		else if (fn.GetExt() == _("mse-locale"))       p = new_shared<Locale>();
		else if (fn.GetExt() == _("mse-include"))      p = new_shared<IncludePackage>();
//		else if (fn.GetExt() == _("mse-symbol-font"))  p = new_shared<SymbolFont>();
		else {
			throw PackageError(_("Unrecognized package type: '") + fn.GetExt() + _("'\nwhile trying to open: ") + name);
		}
		p->open(filename);
		return p;
	}
}

String PackageManager::findFirst(const String& pattern) {
	return wxFindFirstFile(data_directory + _("/") + pattern, 0);
}

InputStreamP PackageManager::openFileFromPackage(const String& name) {
	// we don't want an absolute path (for security reasons)
	String n;
	if (!name.empty() && name.GetChar(0) == _('/')) n = name.substr(1);
	else                                            n = name;
	// break
	size_t pos = n.find_first_of(_("/\\"));
	if (pos == String::npos) throw FileNotFoundError(n, _("No package name specified, use 'package/filename'"));
	// open package and file
	PackagedP p = openAny(n.substr(0, pos));
	return p->openIn(n.substr(pos+1));
}

void PackageManager::destroy() {
	loaded_packages.clear();
}