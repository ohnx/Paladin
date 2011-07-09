#include "PObject.h"
#include "PArgs.h"
#include "PObjectBroker.h"

#include <ClassInfo.h>
#include <stdio.h>

PObject::PObject(void)
	:	fType("PObject"),
		fFriendlyType("Generic Object")
{
	fPropertyList = new BObjectList<PropertyData>(20,true);
	fInheritedList = new BObjectList<PMethod>(20,true);
	fInterfaceList = new BObjectList<BString>(20,true);
	fMethodList = new BObjectList<PMethod>(20,true);
	fEventList = new BObjectList<EventData>(20,true);
	
	PObjectBroker::RegisterObject(this);
	
	AddProperty(new IntProperty("ObjectID", GetID(), "Unique identifier of the object"),
				PROPERTY_READ_ONLY);
	AddEvent("Destroy", "The object is about to be destroyed.");
}


PObject::PObject(BMessage *msg)
	:	fType("PObject")
{
	fPropertyList = new BObjectList<PropertyData>(20,true);
	fInheritedList = new BObjectList<PMethod>(20,true);
	fInterfaceList = new BObjectList<BString>(20,true);
	fMethodList = new BObjectList<PMethod>(20,true);
	fEventList = new BObjectList<EventData>(20,true);
	
	PObjectBroker::RegisterObject(this);
	if (msg->FindString("type",&fType) != B_OK)
		fType = "PObject";
	
	int32 i = 0; 
	BMessage propmsg;
	while (msg->FindMessage("property",i++,&propmsg) == B_OK)
	{
		BString ptype;
		if (propmsg.FindString("type",&ptype) != B_OK)
			continue;
		PProperty *p = gPropertyRoster.MakeProperty(ptype.String(),&propmsg);
		if (p)
			AddProperty(p);
	}
	
	RemoveProperty(FindProperty("ObjectID"));
	AddProperty(new IntProperty("ObjectID", GetID(), "Unique identifier of the object"),
				PROPERTY_READ_ONLY);
	AddEvent("Destroy", "The object is about to be destroyed.");
}


PObject::PObject(const char *name)
	:	fType("PObject")
{
	fPropertyList = new BObjectList<PropertyData>(20,true);
	fInheritedList = new BObjectList<PMethod>(20,true);
	fInterfaceList = new BObjectList<BString>(20,true);
	fMethodList = new BObjectList<PMethod>(20,true);
	fEventList = new BObjectList<EventData>(20,true);
	
	PObjectBroker::RegisterObject(this);
	AddProperty(new IntProperty("ObjectID", GetID(), "Unique identifier of the object"),
				PROPERTY_READ_ONLY);
	AddProperty(new StringProperty("Name",name));
	AddEvent("Destroy", "The object is about to be destroyed.");
}


PObject::PObject(const PObject &from)
	:	fType("PObject")
{
	fPropertyList = new BObjectList<PropertyData>(20,true);
	fInheritedList = new BObjectList<PMethod>(20,true);
	fInterfaceList = new BObjectList<BString>(20,true);
	fMethodList = new BObjectList<PMethod>(20,true);
	fEventList = new BObjectList<EventData>(20,true);
	
	PObjectBroker::RegisterObject(this);
	*this = from;
}


PObject &
PObject::operator=(const PObject &from)
{
	fPropertyList->MakeEmpty();
	for (int32 i = 0; i < from.CountProperties(); i++)
		AddProperty(from.PropertyAt(i)->Duplicate());
	fType = from.fType;
	RemoveProperty(FindProperty("ObjectID"));
	AddProperty(new IntProperty("ObjectID", GetID(), "Unique identifier of the object"),
				PROPERTY_READ_ONLY);
	
	fInheritedList->MakeEmpty();
	for (int32 i = 0; i < from.CountInheritedMethods(); i++)
		AddInheritedMethod(new PMethod(*from.MethodAt(i)));
	
	fInterfaceList->MakeEmpty();
	for (int32 i = 0; i < from.CountInterfaces(); i++)
		AddInterface(from.InterfaceAt(i));
	
	fMethodList->MakeEmpty();
	for (int32 i = 0; i < from.CountMethods(); i++)
		AddMethod(new PMethod(*from.MethodAt(i)));
	
	fEventList->MakeEmpty();
	for (int32 i = 0; i < from.CountEvents(); i++)
		AddEvent(new EventData(*from.EventAt(i)));
	
	
	return *this;
}


PProperty *
PObject::operator[](const char *name)
{
	return FindProperty(name);
}


PProperty *
PObject::operator[](const BString &name)
{
	return FindProperty(name);
}


PObject::~PObject(void)
{
	PArgs in, out;
	RunEvent("Destroy", in.ListRef(), out.ListRef());
	
	PObjectBroker *broker = PObjectBroker::GetBrokerInstance();
	broker->UnregisterObject(this);
	
	delete fPropertyList;
	delete fInterfaceList;
	delete fMethodList;
	delete fEventList;
}


PObject *
PObject::Create(void)
{
	return new PObject();
}


PObject *
PObject::Duplicate(void) const
{
	return new PObject(*this);
}

	
BArchivable *
PObject::Instantiate(BMessage *data)
{
	if (validate_instantiation(data, "PObject"))
		return new PObject(data);

	return NULL;
}


status_t
PObject::Archive(BMessage *data, bool deep) const
{
	status_t status = BArchivable::Archive(data, deep);
	status = data->AddString("type",fType);
	if (status != B_OK)
		return status;
	
	for (int32 i = 0; i < CountProperties(); i++)
	{
		PropertyData *d = fPropertyList->ItemAt(i);
		BMessage msg;
		d->value->Archive(&msg);
		status = data->AddMessage("property",&msg);
		if (status != B_OK)
			return status;
		status = data->AddInt32("propertyflags",d->flags);
		if (status != B_OK)
			return status;
	}
	
	if (status == B_OK)
		status = data->AddString("class",fType.String());
	
	return status;
}


uint64
PObject::GetID(void) const
{
	return fObjectID;
}


status_t
PObject::RunMethod(const char *name, PArgList &in, PArgList &out, void *extraData)
{
	PMethod *method = FindMethod(name);
	if (!method)
		return B_NAME_NOT_FOUND;
	
	method->Run(this, in, out, extraData);
	return B_OK;
}


PMethod *
PObject::FindMethod(const char *name)
{
	if (!name)
		return NULL;
	
	for (int32 i = 0; i < fMethodList->CountItems(); i++)
	{
		PMethod *item = fMethodList->ItemAt(i);
		if (item->GetName().ICompare(name) == 0)
			return item;
	}
	return NULL;
}


PMethod *
PObject::MethodAt(const int32 &index) const
{
	return fMethodList->ItemAt(index);
}


int32
PObject::CountMethods(void) const
{
	return fMethodList->CountItems();
}


status_t
PObject::RunInheritedMethod(const char *name, PArgList &in, PArgList &out,
							void *extraData)
{
	PMethod *method = FindInheritedMethod(name);
	if (!method)
		return B_NAME_NOT_FOUND;
	
	method->Run(this, in, out, extraData);
	return B_OK;
}


PMethod *
PObject::FindInheritedMethod(const char *name)
{
	if (!name)
		return NULL;
	
	for (int32 i = 0; i < fInheritedList->CountItems(); i++)
	{
		PMethod *item = fInheritedList->ItemAt(i);
		if (item->GetName().ICompare(name) == 0)
			return item;
	}
	return NULL;
}


PMethod *
PObject::InheritedMethodAt(const int32 &index) const
{
	return fInheritedList->ItemAt(index);
}


int32
PObject::CountInheritedMethods(void) const
{
	return fInheritedList->CountItems();
}


BString
PObject::GetType(void) const
{
	return fType;
}

			
BString
PObject::GetFriendlyType(void) const
{
	return fFriendlyType;
}

			
bool
PObject::UsesInterface(const char *name)
{
	if (!name)
		return false;
	
	for (int32 i = 0; i < fInterfaceList->CountItems(); i++)
	{
		BString *str = fInterfaceList->ItemAt(i);
		if (str && str->ICompare(name) == 0)
			return true;
	}
	return false;
}


bool
PObject::UsesInterface(const BString &name)
{
	return UsesInterface(name.String());
}


BString
PObject::InterfaceAt(const int32 &index) const
{
	BString *str = fInterfaceList->ItemAt(index);
	return str ? *str : BString();
}


int32
PObject::CountInterfaces(void) const
{
	return fInterfaceList->CountItems();
}


EventData *
PObject::EventAt(const int32 &index) const
{
	return fEventList->ItemAt(index);
}


int32
PObject::CountEvents(void) const
{
	return fEventList->CountItems();
}

			
void
PObject::PrintToStream(void)
{
	printf("Object:\nInterfaces:\n");
	for (int32 i = 0; i < CountInterfaces(); i++)
		printf("\t%s\n",InterfaceAt(i).String());
	
	printf("Properties:\n");
	if (CountProperties() == 0)
		printf("\tNone\n");
	for (int32 i = 0; i < CountProperties(); i++)
	{
		PProperty *p = PropertyAt(i);
		printf("\t%s (%s): %s\n",p->GetName().String(),
				p->GetType().String(), p->GetValueAsString().String());
	}
	
	printf("Methods:\n");
	if (CountMethods() == 0)
		printf("\tNone\n");
	for (int32 i = 0; i < CountMethods(); i++)
		printf("\t%s\n",MethodAt(i)->GetName().String());
	
	printf("Inherited Methods:\n");
	if (CountInheritedMethods() == 0)
		printf("\tNone\n");
	for (int32 i = 0; i < CountInheritedMethods(); i++)
		printf("\t%s\n",InheritedMethodAt(i)->GetName().String());
	
	printf("Events:\n");
	if (CountEvents() == 0)
		printf("\tNone\n");
	for (int32 i = 0; i < CountEvents(); i++)
		printf("\t%s\n",EventAt(i)->name.String());
}


void
PObject::ConvertMsgToArgs(BMessage &in, PArgList &out)
{
	char *fieldName;
	uint32 fieldType;
	int32 fieldCount;
	
	empty_parglist(&out);
	
	int32 i = 0;
	while (in.GetInfo(B_ANY_TYPE, i, &fieldName, &fieldType, &fieldCount) == B_OK)
	{
		for (int32 j = 0; j < fieldCount; j++)
		{
			void *ptr = NULL;
			ssize_t size = 0;
			in.FindData(fieldName, fieldType, j, (const void**)&ptr, &size);
			
			PArgType pargType;
			switch (fieldType)
			{
				case B_INT8_TYPE:
					pargType = PARG_INT8;
					break;
				case B_INT16_TYPE:
					pargType = PARG_INT16;
					break;
				case B_INT32_TYPE:
					pargType = PARG_INT32;
					break;
				case B_INT64_TYPE:
					pargType = PARG_INT64;
					break;
				case B_FLOAT_TYPE:
					pargType = PARG_FLOAT;
					break;
				case B_DOUBLE_TYPE:
					pargType = PARG_DOUBLE;
					break;
				case B_BOOL_TYPE:
					pargType = PARG_BOOL;
					break;
				case B_CHAR_TYPE:
					pargType = PARG_CHAR;
					break;
				case B_STRING_TYPE:
					pargType = PARG_STRING;
					break;
				case B_RECT_TYPE:
					pargType = PARG_RECT;
					break;
				case B_POINT_TYPE:
					pargType = PARG_POINT;
					break;
				case B_RGB_COLOR_TYPE:
					pargType = PARG_COLOR;
					break;
				case B_POINTER_TYPE:
					pargType = PARG_POINTER;
					break;
				default:
					pargType = PARG_RAW;
					break;
			}
			add_parg(&out, fieldName, ptr, size, pargType);
		}
		
		i++;
	}
	add_parg_int32(&out, "what", in.what);
}


void
PObject::ConvertArgsToMsg(PArgList &in, BMessage &out)
{
	PArgListItem *item = get_parg_first(&in);
	out.MakeEmpty();
	
	while  (item)
	{
		switch (item->type)
		{
			case PARG_INT8:
			{
				out.AddInt8(item->name, *((int8*)item->data));
				break;
			}
			case PARG_INT16:
			{
				out.AddInt16(item->name, *((int16*)item->data));
				break;
			}
			case PARG_INT32:
			{
				out.AddInt32(item->name, *((int32*)item->data));
				break;
			}
			case PARG_INT64:
			{
				out.AddInt64(item->name, *((int64*)item->data));
				break;
			}
			case PARG_FLOAT:
			{
				out.AddFloat(item->name, *((float*)item->data));
				break;
			}
			case PARG_DOUBLE:
			{
				out.AddDouble(item->name, *((double*)item->data));
				break;
			}
			case PARG_BOOL:
			{
				out.AddBool(item->name, *((bool*)item->data));
				break;
			}
			case PARG_CHAR:
			{
				out.AddInt8(item->name, *((int8*)item->data));
				break;
			}
			case PARG_STRING:
			{
				out.AddString(item->name, (char*)item->data);
				break;
			}
			case PARG_RECT:
			{
				out.AddRect(item->name, *((BRect*)item->data));
				break;
			}
			case PARG_POINT:
			{
				out.AddPoint(item->name, *((BPoint*)item->data));
				break;
			}
			case PARG_COLOR:
			{
				rgb_color c;
				uint8 *p = (uint8*)item->data;
				c.red = p[0];
				c.green = p[1];
				c.blue = p[2];
				c.alpha = p[3];
				
				out.AddData(item->name, B_RGB_COLOR_TYPE, (const void**)&c,
							sizeof(rgb_color));
				break;
			}
			case PARG_POINTER:
			{
				out.AddPointer(item->name, item->data);
				break;
			}
			default:
				break;
		}
	}
}


void
PObject::AddInterface(const char *name)
{
	if (!name || UsesInterface(name))
		return;
	
	fInterfaceList->AddItem(new BString(name));
}


void
PObject::RemoveInterface(const char *name)
{
	if (!name)
		return;
	
	for (int32 i = 0; i < fInterfaceList->CountItems(); i++)
	{
		BString *str = fInterfaceList->ItemAt(i);
		if (str && str->ICompare(name) == 0)
		{
			fInterfaceList->RemoveItemAt(i);
			delete str;
			return;
		}
	}
}


status_t
PObject::AddMethod(PMethod *method)
{
	if (!method)
		return B_ERROR;
	
	if (FindMethod(method->GetName().String()))
		return B_NAME_IN_USE;
	
	fMethodList->AddItem(method);
	
	return B_OK;
}


status_t
PObject::RemoveMethod(const char *name)
{
	if (!name)
		return B_ERROR;
	
	for (int32 i = 0; i < fMethodList->CountItems(); i++)
	{
		PMethod *item = fMethodList->ItemAt(i);
		if (item->GetName().ICompare(name) == 0)
		{
			fMethodList->RemoveItemAt(i);
			return B_OK;
		}
	}
	
	return B_NAME_NOT_FOUND;
}


status_t
PObject::ReplaceMethod(const char *old, PMethod *newMethod)
{
	if (!old)
		return B_ERROR;
	
	RemoveMethod(old);
	
	if (newMethod)
		fMethodList->AddItem(newMethod);
	return B_OK;
}


status_t
PObject::AddInheritedMethod(PMethod *method)
{
	if (!method)
		return B_ERROR;
	
	if (FindInheritedMethod(method->GetName().String()))
		return B_NAME_IN_USE;
	
	fInheritedList->AddItem(method);
	
	return B_OK;
}


status_t
PObject::AddEvent(const char *name, const char *description,
				PMethodInterface *interface)
{
	if (!name || !description)
		return B_NAME_IN_USE;
	
	if (FindEvent(name))
		return B_OK;
	
	return AddEvent(new EventData(name, description, interface));
}


status_t
PObject::AddEvent(EventData *data)
{
	if (fEventList->HasItem(data))
		return B_OK;
	
	fEventList->AddItem(data);
	return B_OK;
}


status_t
PObject::RemoveEvent(const char *name)
{
	EventData *data = FindEvent(name);
	if (!data)
		return B_NAME_NOT_FOUND;
	
	return fEventList->RemoveItem(data) ? B_OK : B_ERROR;
}


EventData *
PObject::FindEvent(const char *name)
{
	if (!name)
		return NULL;
	
	for (int32 i = 0; i < fEventList->CountItems(); i++)
	{
		EventData *item = fEventList->ItemAt(i);
		if (item->name.ICompare(name) == 0)
			return item;
	}
	
	return NULL;
}


status_t
PObject::RunEvent(const char *name, PArgList &in, PArgList &out)
{
	if (!name)
		return B_ERROR;
	
	return RunEvent(FindEvent(name), in, out);
}


status_t
PObject::RunEvent(EventData *data, PArgList &in, PArgList &out)
{
	if (!data)
		return B_ERROR;
	
	if (data->hook)
	{
		// This searches for the EventName data from the beginning of
		// the parglist. We set the EventName argument so that an event
		// handler can act as a dispatcher for multiple events.
		PArgListItem *eventName = find_parg(&in, "EventName", NULL);
		if (eventName)
			set_parg(eventName, data->name.String(), data->name.Length(),
					PARG_STRING);
		else
			add_parg_string(&in, "EventName", data->name.String());
		
		PArgListItem *extraArg = find_parg(&in, "ExtraData", NULL);
		if (extraArg)
			set_parg(extraArg, data->extraData, sizeof(data->extraData),
					PARG_POINTER);
		else
			add_parg_pointer(&in, "ExtraData", data->extraData);
		
		return data->hook(this, &in, &out, data->extraData);
	}
	
	return B_OK;
}


status_t
PObject::ConnectEvent(const char *name, MethodFunction func, void *extraData)
{
	EventData *data = FindEvent(name);
	if (!data)
		return B_NAME_NOT_FOUND;
	
	data->hook = func;
	data->extraData = extraData;
	
	return B_OK;
}


PObject *
MakeObject(const char *type)
{
	PObjectBroker *owner = PObjectBroker::GetBrokerInstance();
	return owner->MakeObject(type);
}


PObject *
UnflattenObject(BMessage *msg)
{
	PObjectBroker *owner = PObjectBroker::GetBrokerInstance();
	BString type;
	if (msg->FindString("class",0,&type) != B_OK)
		return NULL;
	return owner->MakeObject(type.String(),msg);
}


EventData::EventData(const char *n, const char *d, PMethodInterface *pmi, void *ptr)
{
	name = n;
	description = d;
	hook = NULL;
	if (pmi)
		interface = *pmi;
	extraData = ptr;
}


EventData::EventData(const EventData &from)
{
	*this = from;
}


EventData &
EventData::operator=(const EventData &from)
{
	name = from.name;
	description = from.description;
	interface = from.interface;
	hook = from.hook;
	code = from.code;
	
	// NOTE: this is *not* copied
	extraData = from.extraData;
	
	return *this;
}
