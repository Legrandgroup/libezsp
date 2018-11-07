#pragma once

class IObserver {
public:
	IObserver() {};
	virtual ~IObserver() {};

	//Change parameters here if you would like to transmit something else than an int
	//You can also add more that 1 notification method.
	virtual void notify(const int& content) = 0;
};
