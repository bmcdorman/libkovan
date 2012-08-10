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

#include "shm_p.hpp"
#include "private.hpp"
#include <sys/shm.h>
#include <cstring>
#include <cstdio>

using namespace Private;

PublishListener::~PublishListener() {}

Private::SharedMemory *SharedMemoryImpl::sharedMemory()
{
	return instance()->m_shared;
}

Private::SharedMemoryServer *SharedMemoryImpl::sharedMemoryServer()
{
	Private::SharedMemory *shared = sharedMemory();
	return shared ? &shared->server : 0;
}

Private::SharedMemoryClient *SharedMemoryImpl::sharedMemoryClient()
{
	return &instance()->m_client;
}

Private::SharedMemoryInterClient *SharedMemoryImpl::sharedMemoryInterClient()
{
	Private::SharedMemory *shared = sharedMemory();
	return shared ? &shared->interClient : 0;
}

void SharedMemoryImpl::addPublishListener(PublishListener *listener)
{
	m_listeners.push_back(listener);
}

void SharedMemoryImpl::removePublishListener(PublishListener *listener)
{
	std::vector<PublishListener *>::iterator it = m_listeners.begin();
	for(; it != m_listeners.end(); ++it) {
		if(*it == listener) it = m_listeners.erase(it);
	}
}

void SharedMemoryImpl::setAutoPublish(const bool& on)
{
	m_autoPublish = on;
}

bool SharedMemoryImpl::doesAutoPublish() const
{
	return m_autoPublish;
}

void SharedMemoryImpl::doAutoPublish()
{
	if(m_autoPublish) publish();
}

void SharedMemoryImpl::publish()
{
	Private::SharedMemoryClient *client = &sharedMemory()->client;
	pthread_mutex_t *mutex = &sharedMemory()->clientMutex;
	pthread_mutex_lock(mutex);
	memcpy(client, &m_client, sizeof(Private::SharedMemoryClient));
	pthread_mutex_unlock(mutex);
	
	std::vector<PublishListener *>::const_iterator it = m_listeners.begin();
	for(; it != m_listeners.end(); ++it) {
		(*it)->published(&m_client);
	}
	static unsigned char i = 0;
	if(i++ == 0) printf("publish\n");
}

SharedMemoryImpl::SharedMemoryImpl()
	: m_shared(0),
	m_autoPublish(true)
{
	static size_t size = sizeof(SharedMemory);
	int shmid = shmget(SHARED_MEMORY_KEY, size, IPC_CREAT | 0x1b6);
	if(shmid < 0) return;
	
	// Attach to current shared memory segment.
	void *shm = shmat(shmid, 0, 0);
	if((void *) -1 == shm) {
		printf("Could not attach to shared memory with key %u\n", SHARED_MEMORY_KEY);
		return;
	}
	
	printf("Connected to shared memory\n");
	
	m_shared = reinterpret_cast<Private::SharedMemory *>(shm);
}

SharedMemoryImpl::~SharedMemoryImpl()
{
	if(m_shared) shmdt(m_shared);
}

bool SharedMemoryImpl::isErrorState() const
{
	return !m_shared;
}

SharedMemoryImpl *SharedMemoryImpl::instance()
{
	static SharedMemoryImpl instance;
	return &instance;
}
