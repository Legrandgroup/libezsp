#pragma once

#include "../GenericAsyncDataInputObservable.h"

class RaritanUARTInputObservable : public GenericAsyncDataInputObservable {
public:
	RaritanUARTInputObservable();
	virtual ~RaritanUARTInputObservable();

	void genRandomData();
};
