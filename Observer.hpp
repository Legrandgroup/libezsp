/*
 * Observer.hpp
 *
 *  Created on: Jun 19, 2015
 *      Author: alex
 */

#ifndef OBSERVER_HPP_
#define OBSERVER_HPP_

class Observer {
public:
	Observer() {};
	virtual ~Observer() {};

	//Change parameters here if you would like to transmit something else than an int
	//You can also add more that 1 notification method.
	virtual void notify(const int& content) = 0;
};

#endif /* OBSERVER_HPP_ */
