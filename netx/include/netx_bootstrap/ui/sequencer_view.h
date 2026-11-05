#pragma once

#include "detail/componentview.h"




//
//Primary API

namespace Netx
{

	class AbstractSequencerView;

	class PatternEditor;

	class ModulationEditor;

}




//
//AbstractSequencerView

class Netx::AbstractSequencerView : public Detail::ComponentView
{
public:

	REFLEX_OBJECT(Netx::AbstractSequencerView, GLX::Object);

	using Pattern = AbstractSequencer::Sequence;

	

	//lifetime

	AbstractSequencerView(AbstractSequencer & sequencer);

	~AbstractSequencerView();



	//setup

	void SetPopulateMenuCallback(const Function<bool(GLX::Menu & menu,Key32 ctx)> & callback);

	void SetEventLabelCallback(const Function<WString(const AbstractSequencer::Event & e)> & callback);


	void SetStepSize(UInt grid_step);

	UInt GetStepSize() const;


	void SelectPattern(UInt idx);

	Idx GetSelectedPattern() const;



protected:

	[[nodiscard]] virtual Unretained <GLX::Detail::Graphic> CreateGraphic(GLX::Size size) = 0;


	void OnReset(Key32 context) override;

	void OnRestore(Data::Archive::View & stream, Key32 context) override;

	void OnStore(Data::Archive & stream) const override;


	void OnAttachWindow() override;

	void OnDetachWindow() override;

	void OnBuildLayout(AccommodateFn & accommodate, AlignFn & align) override;

	void OnUpdate() override;

	void OnClock(Float) override;

	void OnSetStyle(const GLX::Style & style) override;

	bool OnEvent(GLX::Object & src, GLX::Event & e) override;


	void OnQueryProperty(Address adr, Reflex::Object * & pobject) const override;


	struct Impl;

	Reflex::Detail::Initialiser <Impl,16,384> impl;

};




//
//PatternEditor

class Netx::PatternEditor : public Netx::AbstractSequencerView
{
public:

	REFLEX_OBJECT(Netx::PatternEditor, AbstractSequencerView);

	PatternEditor(AbstractSequencer & sequencer);



protected:

	[[nodiscard]] Unretained <GLX::Detail::Graphic> CreateGraphic(GLX::Size size) override;

	bool OnEvent(GLX::Object & src, GLX::Event & e) override;

	GLX::Core::Trap OnPointerTender(GLX::Core::PointerAction mouseaction, const GLX::Core::Pointer & pointer, UInt8 flags) override;


	template <bool LABELS> void DrawEvents(GLX::ColourPoints & unselected, GLX::ColourPoints & selected, Array < Pair < ConstReference <GLX::Graphic>, GLX::Colour> > * graphics);

	bool OnMouseDown(GLX::Event & e);
	bool OnMouseDrag(GLX::Event & e);
	bool OnMouseUp(GLX::Event & e);

	void ApplyDrag(GLX::Point delta_px, bool fine);

};




//
//ModulationEditor

class Netx::ModulationEditor : public Netx::AbstractSequencerView
{
public:

	REFLEX_OBJECT(Netx::ModulationEditor, AbstractSequencerView);

	struct Lane
	{
		WString name = L"Modulation";

		//Return true if event should be shown in this lane (if null, uses iPattern::Filter()).
		FunctionPointer<bool(const AbstractSequencer::Event & e, UInt songpos)> filter = nullptr;

		//Read value from extended (0..1 recommended).
		FunctionPointer<Float(const AbstractSequencer::Event & e)> get_value = nullptr;

		//Write value to extended.
		FunctionPointer<void(Data::Archive::Region extended, Float value)> set_value = nullptr;
	};

	ModulationEditor(AbstractSequencer & sequencer);

	void SetLane(const Lane & lane);

	const Lane & GetLane() const;

	bool OnEvent(GLX::Object & src, GLX::Event & e) override;

	

private:

	enum Action : UInt8
	{
		kActionNone,
		kActionDragValue,
		kActionRubberband
	};

	[[nodiscard]] Unretained <GLX::Detail::Graphic> CreateGraphic(GLX::Size size) override;

	void DrawBars(GLX::ColourPoints & unselected, GLX::ColourPoints & selected, const GLX::Size & size);

	bool OnMouseDown(GLX::Event & e);
	bool OnMouseDrag(GLX::Event & e);
	bool OnMouseUp(GLX::Event & e);
	bool OnKeyDown(GLX::Event & e);

	void ApplyValueDrag(const GLX::Point & mouse_local);

	Float MouseToValue(const GLX::Point & mouse_local, const GLX::Size & size) const;


private:

	Lane m_lane;

	bool m_mouse_down = false;
	Action m_action = kActionNone;

	GLX::Point m_drag_anchor;
};




//
//Secondary API

namespace Netx
{

	bool IsTriplet(UInt ppq, UInt grid);	//heuristic

	Pair <WString,bool> MakeGridDescription(UInt ppq, UInt grid);

	void PopulateGridMenu(GLX::Menu & menu, UInt32 ppq, UInt32 grid, const Function <void(UInt32)> & callback);

	void ToggleTriplets(UInt32 ppq, UInt32 grid, const Function <void(UInt32)> & callback);

}




//
//impl

inline bool Netx::IsTriplet(UInt ppq, UInt grid)
{
	auto div = ppq / 16;

	return ((grid / div) * div) != grid;
}
