#pragma once

#include "detail/overlay.h"




//
//declarations

REFLEX_NS(DSP)

class SampleDisplay;

REFLEX_END




//
//SampleDisplay

class DSP::SampleDisplay : public GLX::ZoomArea
{
public:

	REFLEX_OBJECT(SampleDisplay, GLX::ZoomArea);


	//declarations

	class TrimBehaviour;

	class NameDisplay;

	typedef ObjectOf < Array <SampleRef> > DragSamples;

	

	//msgs

	REFLEX_DECLARE_KEY32(SampleDisplayOpen);	//sample: const DSP::Sample&
	REFLEX_DECLARE_KEY32(sample);

	REFLEX_DECLARE_KEY32(SampleDisplayResolveMissing);
	REFLEX_DECLARE_KEY32(id);



	//lifetime

	SampleDisplay(File::ResourcePool & resourcepool);

	~SampleDisplay();


	
	//setup

	void SetAxis(bool yaxis);

	void ShowChannels(bool left, bool right);



	//sample
		
	void ClearSample();
	 
	bool SetSample(const Sample & sample, Pair <UInt> region = { 0, kMaxUInt32 });

	ConstAlreadyRetained <Sample> GetSample() const;


	void OpenSample();	//file dialog

	void EnableOpenSample(bool enable);



	//view

	void ZoomOut();



	//io
	
	void Serialize(Data::Archive & stream) const;

	void Deserialize(Data::Archive::View & stream);

	void Reset();



	//info

	Int GetSnapSensitivity() const;



	//elements

	AlreadyRetained <File::ResourcePool> resourcepool;

	AlreadyRetained <GLX::Object> GetContent() { return m_content; }

	Detail::Overlay overlay;



protected:

	void OnSetStyle(const GLX::Style & style) override;

	bool OnEvent(GLX::Object & src, GLX::Event & e) override;

	void OnUpdate() override;



private:

	bool ShowContextMenu(const GLX::Event & e);


	bool m_yaxis;

	bool m_open_sample_enabled;

	UInt8 m_channel_flags;
	
	
	ConstReference <Sample> m_sample;

	Tuple <UInt,UInt> m_region;
		

	GLX::Object m_content;

	GLX::Button m_channel_button;


	GLX::Range m_vor;

};




//
//SampleDisplay::TrimBehaviour (display with ViewPortRegion)

class DSP::SampleDisplay::TrimBehaviour : public GLX::Object::Delegate
{
public:

	REFLEX_OBJECT(TrimBehaviour, GLX::Object::Delegate);
	
	enum MouseArea : UInt8 { kMouseAreaBackground, kMouseAreaRange, kMouseAreaStart, kMouseAreaEnd, kNumMouseArea };


	[[nodiscard]] static Unretained <TrimBehaviour> Create(SampleDisplay & sampledisplay, Key32 id = K32("trim"));

	static void Discard(SampleDisplay & sampledisplay, Key32 id = K32("trim"));

	
	virtual void EnableMove(bool enable) = 0;

	virtual void SetRange(UInt start, UInt length, UInt bounds_start = 0, UInt bounds_length = kMaxUInt32) = 0;

	virtual Pair <UInt> GetRange() const = 0;

	virtual MouseArea GetMouseArea(const GLX::Point & mousepos) const = 0;


	const AlreadyRetained <SampleDisplay> sampledisplay;

	const Key32 id;



protected:

	TrimBehaviour(SampleDisplay & sampledisplay, Key32 id);

};




//
//SampleDisplay::NameDisplay

class DSP::SampleDisplay::NameDisplay : public GLX::Object
{
public:

	//lifetime

	NameDisplay(SampleDisplay & sampledisplay);



private:

	bool OnEvent(GLX::Object & src, GLX::Event & e) override;

	void OnSetStyle(const GLX::Style & style) override;

	void OnUpdate() override;


	SampleDisplay & sampledisplay;

	GLX::Button m_browse;

	GLX::Label m_label;

};




//
//Detail

REFLEX_NS(DSP::Detail)

WString MakeUniqueFilename(const WString & path);	//number will be appended to make a file that doesnt exist

REFLEX_END




//
//impl

REFLEX_INLINE ConstAlreadyRetained <DSP::Sample> DSP::SampleDisplay::GetSample() const
{
	return m_sample;
}

inline void DSP::SampleDisplay::TrimBehaviour::Discard(SampleDisplay & sampledisplay, Key32 id)
{
	sampledisplay.GetContent()->ClearDelegate(id);
}
