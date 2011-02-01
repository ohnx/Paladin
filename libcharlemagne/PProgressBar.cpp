#include "PProgressBar.h"
#include <Application.h>
#include <View.h>
#include <Window.h>

#include "CInterface.h"
#include "EnumProperty.h"
#include "PArgs.h"


class PProgressBarBackend : public BStatusBar
{
public:
			PProgressBarBackend(PObject *owner);
	void	AttachedToWindow(void);
	void	AllAttached(void);
	void	DetachedFromWindow(void);
	void	AllDetached(void);
	
	void	MakeFocus(bool value);
	
	void	FrameMoved(BPoint pt);
	void	FrameResized(float w, float h);
	
	void	KeyDown(const char *bytes, int32 count);
	void	KeyUp(const char *bytes, int32 count);
	
	void	MouseDown(BPoint pt);
	void	MouseMoved(BPoint pt, uint32 buttons, const BMessage *msg);
	void	MouseUp(BPoint pt);
	
	void	WindowActivated(bool active);
	
	void	Draw(BRect update);
	void	DrawAfterChildren(BRect update);
	void	MessageReceived(BMessage *msg);
	
private:
	PObject	*fOwner;
};

PProgressBar::PProgressBar(void)
	:	PView()
{
	fType = "PProgressBar";
	fFriendlyType = "Progress Bar";
	AddInterface("PProgressBar");
	
	InitProperties();
	InitMethods();
	InitBackend();
}


PProgressBar::PProgressBar(BMessage *msg)
	:	PView(msg)
{
	fType = "PProgressBar";
	fFriendlyType = "ProgressBar";
	AddInterface("PProgressBar");
	
	BMessage viewmsg;
	if (msg->FindMessage("backend",&viewmsg) == B_OK)
		fView = (BStatusBar*)BStatusBar::Instantiate(&viewmsg);
	
	InitBackend();
}


PProgressBar::PProgressBar(const char *name)
	:	PView(name)
{
	fType = "PProgressBar";
	fFriendlyType = "ProgressBar";
	AddInterface("PProgressBar");
	InitMethods();
	InitBackend();
}


PProgressBar::PProgressBar(const PProgressBar &from)
	:	PView(from)
{
	fType = "PProgressBar";
	fFriendlyType = "ProgressBar";
	AddInterface("PProgressBar");
	InitMethods();
	InitBackend();
}


PProgressBar::~PProgressBar(void)
{
	// We don't have to worry about removing and deleting fView -- ~PView does that for us. :)
}


BArchivable *
PProgressBar::Instantiate(BMessage *data)
{
	if (validate_instantiation(data, "PProgressBar"))
		return new PProgressBar(data);

	return NULL;
}


status_t
PProgressBar::GetProperty(const char *name, PValue *value, const int32 &index) const
{
	if (!name || !value)
		return B_ERROR;
	
	BString str(name);
	PProperty *prop = FindProperty(name,index);
	if (!prop)
		return B_NAME_NOT_FOUND;
	
	if (fView->Window())
		fView->Window()->Lock();
	
	BStatusBar *view = dynamic_cast<BStatusBar*>(fView);
		
	if (str.ICompare("PropertyOne") == 0)
	{
		// call backend method here
	}
	else
	{
		if (fView->Window())
			fView->Window()->Unlock();
		return PView::GetProperty(name,value,index);
	}
	
	if (fView->Window())
		fView->Window()->Unlock();
	
	return prop->GetValue(value);
}


status_t
PProgressBar::SetProperty(const char *name, PValue *value, const int32 &index)
{
	if (!name || !value)
		return B_ERROR;
	
	BString str(name);
	PProperty *prop = FindProperty(name,index);
	if (!prop)
		return B_NAME_NOT_FOUND;
	
	if (FlagsForProperty(prop) & PROPERTY_READ_ONLY)
		return B_READ_ONLY;
	
	BoolValue bv;
	ColorValue cv;
	FloatValue fv;
	RectValue rv;
	PointValue pv;
	IntValue iv;
	StringValue sv;
	
	status_t status = prop->SetValue(value);
	if (status != B_OK)
		return status;
	
	if (fView->Window())
		fView->Window()->Lock();
	
	BStatusBar *view = dynamic_cast<BStatusBar*>(fView);
	
	if (str.ICompare("PropertyOne") == 0)
	{
		// prop->GetValue(&sv);
		// Invoke backend method here
	}
	else
	{
		if (fView->Window())
			fView->Window()->Unlock();
		return PView::SetProperty(name,value,index);
	}
	
	if (fView->Window())
		fView->Window()->Unlock();
	
	return prop->GetValue(value);
}


PObject *
PProgressBar::Create(void)
{
	return new PProgressBar();
}


PObject *
PProgressBar::Duplicate(void) const
{
	return new PProgressBar(*this);
}

void
PProgressBar::InitBackend(void)
{
	if (!fView)
		fView = new PProgressBarBackend(this);
	StringValue sv("A generalized View-based control template.");
	SetProperty("Description",&sv);
}


void
PProgressBar::InitProperties(void)
{
/*
	Properties:
		BarColor
		BarHeight
		Label
		CurrentValue
		MaxValue
		Text
		TrailingLabel
		TrailingText
*/
}


void
PProgressBar::InitMethods(void)
{
}


PProgressBarBackend::PProgressBarBackend(PObject *owner)
	:	BStatusBar(BRect(0,0,1,1), "ProgressBarBackend"),
		fOwner(owner)
{
	
}


void
PProgressBarBackend::AttachedToWindow(void)
{
	PArgs in, out;
	EventData *data = fOwner->FindEvent("AttachedToWindow");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
	{
		BStatusBar::AttachedToWindow();
		fOwner->SetColorProperty("BackColor",ViewColor());
	}
}


void
PProgressBarBackend::AllAttached(void)
{
	PArgs in, out;
	EventData *data = fOwner->FindEvent("AllAttached");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BStatusBar::AllAttached();
}


void
PProgressBarBackend::DetachedFromWindow(void)
{
	PArgs in, out;
	EventData *data = fOwner->FindEvent("DetachedFromWindow");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BStatusBar::DetachedFromWindow();
}


void
PProgressBarBackend::AllDetached(void)
{
	PArgs in, out;
	EventData *data = fOwner->FindEvent("AllDetached");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BStatusBar::AllDetached();
}


void
PProgressBarBackend::MakeFocus(bool value)
{
	PArgs in, out;
	EventData *data = fOwner->FindEvent("FocusChanged");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BStatusBar::MakeFocus(value);
}


void
PProgressBarBackend::FrameMoved(BPoint pt)
{
	PArgs in, out;
	in.AddPoint("where", pt);
	
	EventData *data = fOwner->FindEvent("FrameMoved");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BStatusBar::FrameMoved(pt);
}


void
PProgressBarBackend::FrameResized(float w, float h)
{
	PArgs in, out;
	in.AddFloat("width", w);
	in.AddFloat("height", h);
	EventData *data = fOwner->FindEvent("FrameResized");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BStatusBar::FrameResized(w, h);
}


void
PProgressBarBackend::KeyDown(const char *bytes, int32 count)
{
	PArgs in, out;
	in.AddItem("bytes", (void*)bytes, count, PARG_RAW);
	in.AddInt32("count", count);
	EventData *data = fOwner->FindEvent("KeyDown");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BStatusBar::KeyDown(bytes, count);
}


void
PProgressBarBackend::KeyUp(const char *bytes, int32 count)
{
	PArgs in, out;
	in.AddItem("bytes", (void*)bytes, count, PARG_RAW);
	in.AddInt32("count", count);
	EventData *data = fOwner->FindEvent("KeyUp");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BStatusBar::KeyUp(bytes, count);
}


void
PProgressBarBackend::MouseDown(BPoint pt)
{
	PArgs in, out;
	in.AddPoint("where", pt);
	EventData *data = fOwner->FindEvent("MouseDown");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BStatusBar::MouseDown(pt);
}


void
PProgressBarBackend::MouseUp(BPoint pt)
{
	PArgs in, out;
	in.AddPoint("where", pt);
	EventData *data = fOwner->FindEvent("MouseUp");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BStatusBar::MouseUp(pt);
}


void
PProgressBarBackend::MouseMoved(BPoint pt, uint32 buttons, const BMessage *msg)
{
	PArgs in, out;
	in.AddPoint("where", pt);
	in.AddInt32("buttons", buttons);
	in.AddPointer("message", (void*)msg);
	EventData *data = fOwner->FindEvent("MouseMoved");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BStatusBar::MouseMoved(pt, buttons, msg);
}


void
PProgressBarBackend::WindowActivated(bool active)
{
	PArgs in, out;
	in.AddBool("active", active);
	EventData *data = fOwner->FindEvent("WindowActivated");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BStatusBar::WindowActivated(active);
}


void
PProgressBarBackend::Draw(BRect update)
{
	EventData *data = fOwner->FindEvent("Draw");
	if (!data->hook)
		BStatusBar::Draw(update);
	
	PArgs in, out;
	in.AddRect("update", update);
	fOwner->RunEvent("Draw", in.ListRef(), out.ListRef());
	
	if (IsFocus())
	{
		SetPenSize(5.0);
		SetHighColor(0,0,0);
		SetLowColor(128,128,128);
		StrokeRect(Bounds(),B_MIXED_COLORS);
	}
}


void
PProgressBarBackend::DrawAfterChildren(BRect update)
{
	PArgs in, out;
	in.AddRect("update", update);
	EventData *data = fOwner->FindEvent("DrawAfterChildren");
	if (data->hook)
		fOwner->RunEvent(data, in.ListRef(), out.ListRef());
	else
		BStatusBar::DrawAfterChildren(update);
}


void
PProgressBarBackend::MessageReceived(BMessage *msg)
{
	PProgressBar *view = dynamic_cast<PProgressBar*>(fOwner);
	if (view->GetMsgHandler(msg->what))
	{
		PArgs args;
		view->ConvertMsgToArgs(*msg, args.ListRef());
		if (view->RunMessageHandler(msg->what, args.ListRef()) == B_OK)
			return;
	}
	
	BStatusBar::MessageReceived(msg);
}


int32_t
PProgressBarAttachedToWindow(void *pobject, PArgList *in, PArgList *out)
{
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PView *parent = static_cast<PView*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	BStatusBar *fView = (BStatusBar*)parent->GetView();
	
	if (fView->Window())
		fView->Window()->Lock();
	fView->BStatusBar::AttachedToWindow();
	if (fView->Window())
		fView->Window()->Unlock();
	
	return B_OK;
}


int32_t
PProgressBarDraw(void *pobject, PArgList *in, PArgList *out)
{
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PView *parent = static_cast<PView*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	BStatusBar *fView = (BStatusBar*)parent->GetView();
	
	if (fView->Window())
		fView->Window()->Lock();
	
	PArgs args(in);
	BRect r;
	args.FindRect("update", &r);
	
	fView->BStatusBar::Draw(r);
	
	if (fView->Window())
		fView->Window()->Unlock();
	
	return B_OK;
}


int32_t
PProgressBarFrameResized(void *pobject, PArgList *in, PArgList *out)
{
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PView *parent = static_cast<PView*>(pobject);
	if (!parent)
		return B_BAD_TYPE;
	
	BStatusBar *fView = (BStatusBar*)parent->GetView();
	
	if (fView->Window())
		fView->Window()->Lock();
	
	float w,h;
	find_parg_float(in, "width", &w);
	find_parg_float(in, "height", &h);
	
	fView->BStatusBar::FrameResized(w, h);
	
	if (fView->Window())
		fView->Window()->Unlock();
	
	return B_OK;
}
