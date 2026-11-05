#pragma once

#include "../propertydef.h"




//
//Primary API

namespace Netx
{

	enum TransactionStage
	{
		kBegin,
		kEdit,
		kEnd,
	};

	class AbstractPropertySet;

}




//	
//AbstractPropertySet

class Netx::AbstractPropertySet : 
	public Component,
	public Signal <TransactionStage,Address>
{
public:

	using ComponentType = AbstractPropertySet;

	using Component::Component;

	struct Transaction :
		public Object,
		public Module::TransactionScope,
		public Component::Record
	{
	public:

		REFLEX_OBJECT(Netx::AbstractPropertySet::Transaction, Object);

		~Transaction();


		const AlreadyRetained <AbstractPropertySet> propertyset;

		const Address adr;

		const bool notify;


	protected:

		Transaction(AbstractPropertySet & component, Address adr, const WString::View & desc, bool notify = true);

	};



	//content

	Unretained <Object> CreateTransactionListener(const Function <void(TransactionStage,Address)> & callback);



protected:

	void Notify(TransactionStage stage, Address adr);

};

