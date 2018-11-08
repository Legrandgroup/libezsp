#pragma once

#include "../GenericAsyncDataInputObservable.h"

class RaritanUartInputObservable : public GenericAsyncDataInputObservable {
public:
	RaritanUartInputObservable();
	virtual ~RaritanUartInputObservable();

	void genRandomData();
};
