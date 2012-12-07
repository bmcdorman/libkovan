/**************************************************************************
 *  Copyright 2012 KISS Institute for Practical Robotics                  *
 *                                                                        *
 *  This file is part of libkovan.                                        *
 *                                                                        *
 *  libkovan is free software: you can redistribute it and/or modify      *
 *  it under the terms of the GNU General Public License as published by  *
 *  the Free Software Foundation, either version 2 of the License, or     *
 *  (at your option) any later version.                                   *
 *                                                                        *
 *  libkovan is distributed in the hope that it will be useful,           *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *  GNU General Public License for more details.                          *
 *                                                                        *
 *  You should have received a copy of the GNU General Public License     *
 *  along with libkovan. Check the LICENSE file in the project root.      *
 *  If not, see <http://www.gnu.org/licenses/>.                           *
 **************************************************************************/

#include "kovan/motors.hpp"
#include "motors_p.hpp"

Motor::Motor(const port_t& port) throw()
	: m_port(port)
{
	// if(!p_motor) throw InvalidPort("Motor ports are through 1 and 4");
}

void Motor::moveAtVelocity(const int& velocity)
{
	
}

void Motor::moveToPosition(const int& speed, const int& goalPos)
{
	
}

void Motor::moveRelativePosition(const int& speed, const int& deltaPos)
{
	
}

void Motor::setPidGains(const short& p, const short& i, const short& d, const short& pd, const short& id, const short& dd)
{
	Private::Motor::instance()->setPid(m_port, p, i, d, pd, id, dd);
}

void Motor::pidGains(short& p, short& i, short& d, short& pd, short& id, short& dd)
{
	Private::Motor::instance()->pid(m_port, p, i, d, pd, id, dd);
}

void Motor::freeze()
{
	Private::Motor::instance()->setPwm(m_port, 100);
	Private::Motor::instance()->setPwmDirection(m_port, Private::Motor::ActiveStop);
}

bool Motor::getMotorDone() const
{
	return false;
}

void Motor::blockMotorDone() const
{
	
}

void Motor::forward()
{
	motor(100);
}

void Motor::backward()
{
	motor(-100);
}

void Motor::motor(int percent)
{
	Private::Motor::instance()->setPwm(m_port, percent);
	if(percent > 0) Private::Motor::instance()->setPwmDirection(m_port, Private::Motor::Forward);
	else if(percent < 0) Private::Motor::instance()->setPwmDirection(m_port, Private::Motor::Reverse);
	else Private::Motor::instance()->setPwmDirection(m_port, Private::Motor::PassiveStop);
}

void Motor::off()
{
	Private::Motor::instance()->stop(m_port);
}

const port_t& Motor::port() const
{
	return m_port;
}

BackEMF::BackEMF(const unsigned char& port) : m_port(port) {}

unsigned short BackEMF::value() const
{
	return Private::Motor::instance()->backEMF(m_port);
}

unsigned char BackEMF::port() const
{
	return m_port;
}
