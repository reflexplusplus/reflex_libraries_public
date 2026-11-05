#pragma once

#include "defines.h"
#include "detail/iundoable.h"




//
//declarations

namespace Netx
{

	class Network;

}




//
//Network

class Netx::Network :
	public Core::Network,
	public Detail::iHistory,
	public Data::History,
	public Signal <>,
	public Protected
{
public:

	static Network & null;



	//types

	using Detail::iHistory::TransactionScope;



	//static
	
	static Data::PropertySet MakeDefaults(UInt maxbuffersize = 128, Float samplerate = 44100.0f, UInt ppq = 384);	//->bootstrap



	//lifetime

	Network(File::ResourcePool & resourcepool, Data::PropertySet && defaults = MakeDefaults());

	~Network();



	//setup

	void EnableAutoRefresh(bool enable = true);


	
	//access

	void Enumerate(const Function <void(Module&)> & callback);

	Object * Query(const Function <Object*(Module&)> & callback);


	void ClearHistory();

	using Data::History::CanUndo;

	using Data::History::Undo;

	using Data::History::CanRedo;

	using Data::History::Redo;

	WString GetHistoryDescription(const Data::Archive::View & item);

	Unretained <Object> CreateTransactionListener(const Function<void()> & callback) { return Signal::CreateListener(callback); }


	
	//resources

	void RelocateResources(File::ResourcePool::Lock & lock, const Map <Address, WString> & relocates);



	//sys

	const AlreadyRetained <File::ResourcePool> resourcepool;



private:

	friend class Module;

	friend class Netx::Component;

	Network(const Network & module);


	void OnCommit(const Records & records) override;

	void OnRestoreHistory(Data::Archive::View stream, bool redo) override final;


	Reference <Object> m_autorefreshclock;

};
