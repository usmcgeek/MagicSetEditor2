//+----------------------------------------------------------------------------+
//| Description:  Magic Set Editor - Program to make Magic (tm) cards          |
//| Copyright:    (C) Twan van Laarhoven and the other MSE developers          |
//| License:      GNU General Public License 2 or later (see file COPYING)     |
//+----------------------------------------------------------------------------+

// ----------------------------------------------------------------------------- : Includes

#include <util/prec.hpp>
#include <gui/about_window.hpp>
#include <gui/util.hpp>
#include <util/version.hpp>
#include <wx/dcbuffer.h>

// ----------------------------------------------------------------------------- : About window

AboutWindow::AboutWindow(Window* parent)
  : wxDialog(parent, wxID_ANY, _TITLE_("about"), wxDefaultPosition, wxSize(510,340), wxCLIP_CHILDREN | wxDEFAULT_DIALOG_STYLE | wxTAB_TRAVERSAL)
  , logo(load_resource_image(_("about")))
{
  // init controls
  wxControl* ok_button = new HoverButton(this, wxID_OK, _("btn_ok"));
  wxSize bs = ok_button->GetSize(), ws = GetClientSize();
  ok_button->Move(ws.GetWidth() - bs.GetWidth() - 10,  ws.GetHeight() - bs.GetHeight() - 10); // align bottom right
}

bool AboutWindow::Layout() {
  return false;
}

void AboutWindow::onPaint(wxPaintEvent& ev) {
  wxBufferedPaintDC dc(this);
  draw(dc);
}

const char* MSE_AUTHORS[] = {"Twan van Laarhoven (twanvl)", "Sean Hunt (coppro)", "Alissa Rao (Lymia)", "modified for 300dpi by Jason Smith (USMCGeek)"};

void AboutWindow::draw(DC& dc) {
  wxSize ws = GetClientSize();
  // draw background
  dc.SetPen  (*wxTRANSPARENT_PEN);
  dc.SetBrush(wxColor(240,247,255));
  dc.DrawRectangle(0, 0, ws.GetWidth(), ws.GetHeight());
  // draw logo
  dc.DrawBitmap(logo,  (ws.GetWidth() -  logo.GetWidth()) / 2, 5);
  // draw version info
  dc.SetFont(wxFont(9, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, _("Arial")));
  int x = 34, y = 110;
  dc.DrawText(_("Version: ") + app_version.toString() + version_suffix, x, y);
  y += 30;
  // license
  dc.DrawText(_("This program is free software and released under the GNU General Public\nLicense version 2 or later."), x, y);
  y += 30+18;
  // draw author info
  dc.DrawText(_("Developed by"), x, y);
  for (auto name : MSE_AUTHORS) {
    y += 18;
    dc.DrawText(name, x + 20, y);
  }
}

BEGIN_EVENT_TABLE(AboutWindow, wxDialog)
  EVT_PAINT      (AboutWindow::onPaint)
END_EVENT_TABLE  ()


// ----------------------------------------------------------------------------- : Button hover effect

HoverButtonBase::HoverButtonBase(Window* parent, int id, bool accepts_focus)
  : wxControl(parent, id, wxDefaultPosition, wxDefaultSize, wxNO_BORDER )
  , accepts_focus(accepts_focus)
  , hover(false), focus(false), mouse_down(false), key_down(false)
{
  SetBackgroundStyle(wxBG_STYLE_PAINT);
}

void HoverButtonBase::onMouseEnter(wxMouseEvent&) {
  hover = true;
  refreshIfNeeded();
  if (!help_text.empty()) set_status_text(this,help_text);
}
void HoverButtonBase::onMouseLeave(wxMouseEvent&) {
  hover = false;
  refreshIfNeeded();
  if (!help_text.empty()) set_status_text(this,wxEmptyString);
}
void HoverButtonBase::onFocus(wxFocusEvent&) {
  focus = true;
  refreshIfNeeded();
}
void HoverButtonBase::onKillFocus(wxFocusEvent&) {
  focus = false;
  refreshIfNeeded();
}
void HoverButtonBase::onLeftDown(wxMouseEvent&) {
  mouse_down = true;
  SetFocus();
  CaptureMouse();
  refreshIfNeeded();
}
void HoverButtonBase::onLeftUp(wxMouseEvent&) {
  if (HasCapture()) ReleaseMouse();
  mouse_down = false;
  refreshIfNeeded();
  if (hover) {
    onClick();
  }
}
void HoverButtonBase::onLoseCapture(wxMouseCaptureLostEvent&) {
  // We already test for wrong release with HasCapture()
  // but stupid wxWidget people decided to throw assertion failures
}
void HoverButtonBase::onKeyDown(wxKeyEvent& ev) {
  int code = ev.GetKeyCode();
  if (code == WXK_RETURN || code == WXK_SPACE) {
    key_down = true;
    refreshIfNeeded();
  } else {
    ev.Skip();
  }
}
void HoverButtonBase::onKeyUp(wxKeyEvent& ev) {
  int code = ev.GetKeyCode();
  if (code == WXK_RETURN || code == WXK_SPACE) {
    key_down = false;
    refreshIfNeeded();
    onClick();
  }
}

void HoverButtonBase::onClick() {
  wxCommandEvent evt(wxEVT_COMMAND_BUTTON_CLICKED, GetId());
  evt.SetEventObject(this);
  ProcessEvent(evt);
}

bool HoverButtonBase::AcceptsFocus() const {
  return wxControl::AcceptsFocus() && accepts_focus;
}

void HoverButtonBase::refreshIfNeeded() {
  Refresh(false);
}

void HoverButtonBase::onPaint(wxPaintEvent&) {
  wxPaintDC dc(this);
  draw(dc);
}

BEGIN_EVENT_TABLE(HoverButtonBase, wxControl)
  EVT_ENTER_WINDOW   (HoverButtonBase::onMouseEnter)
  EVT_LEAVE_WINDOW   (HoverButtonBase::onMouseLeave)
  EVT_PAINT          (HoverButtonBase::onPaint)
  EVT_SET_FOCUS      (HoverButtonBase::onFocus)
  EVT_KILL_FOCUS     (HoverButtonBase::onKillFocus)
  EVT_LEFT_DOWN      (HoverButtonBase::onLeftDown)
  EVT_LEFT_UP        (HoverButtonBase::onLeftUp)
  EVT_KEY_DOWN       (HoverButtonBase::onKeyDown)
  EVT_KEY_UP         (HoverButtonBase::onKeyUp)
  EVT_MOUSE_CAPTURE_LOST(HoverButtonBase::onLoseCapture)
END_EVENT_TABLE  ()

// ----------------------------------------------------------------------------- : Button with image and hover effect


HoverButton::HoverButton(Window* parent, int id, const String& name, const Color& background, bool accepts_focus)
  : HoverButtonBase(parent, id, accepts_focus)
  , background(background)
  , last_drawn(nullptr)
{
  loadBitmaps(name);
  SetSize(DoGetBestSize());
}

void HoverButton::loadBitmaps(const String& name) {
  if (bitmaps == name) return;
  bitmaps = name;
  bg_normal = Bitmap(load_resource_image(name + _("_normal")));
  bg_hover  = Bitmap(load_resource_image(name + _("_hover")));
  bg_focus  = Bitmap(load_resource_image(name + _("_focus")));
  bg_down   = Bitmap(load_resource_image(name + _("_down")));
  Refresh(false);
}

wxSize HoverButton::DoGetBestSize() const {
  return wxSize(bg_normal.GetWidth(), bg_normal.GetHeight());
}

const Bitmap* HoverButton::toDraw() const {
  return (mouse_down && hover) || key_down ? &bg_down
       : hover                             ? &bg_hover
       : focus                             ? &bg_focus
       :                                     &bg_normal;
}
void HoverButton::refreshIfNeeded() {
  if (last_drawn != toDraw()) Refresh(false);
}

void HoverButton::draw(DC& dc) {
  // clear background (for transparent button images)
  wxSize ws = GetClientSize();
  dc.SetPen(*wxTRANSPARENT_PEN);
  dc.SetBrush(wxBrush(background.Alpha()>0 ? wxColour(background) : wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE)));
  dc.DrawRectangle(0, 0, ws.GetWidth(), ws.GetHeight());
  // draw button
  dc.DrawBitmap(*toDraw(), 0, 0, true);
  last_drawn = toDraw();
}
int HoverButton::drawDelta() const {
  return (mouse_down && hover) || key_down ? 2 : 0;
}
