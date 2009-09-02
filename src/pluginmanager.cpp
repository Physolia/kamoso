/*************************************************************************************
 *  Copyright (C) 2008-2009 by Aleix Pol <aleixpol@gmail.com>                        *
 *  Copyright (C) 2008-2009 by Alex Fiestas <alex@eyeos.org>                         *
 *                                                                                   *
 *  This program is free software; you can redistribute it and/or                    *
 *  modify it under the terms of the GNU General Public License                      *
 *  as published by the Free Software Foundation; either version 2                   *
 *  of the License, or (at your option) any later version.                           *
 *                                                                                   *
 *  This program is distributed in the hope that it will be useful,                  *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of                   *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                    *
 *  GNU General Public License for more details.                                     *
 *                                                                                   *
 *  You should have received a copy of the GNU General Public License                *
 *  along with this program; if not, write to the Free Software                      *
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA   *
 *************************************************************************************/

#include "pluginmanager.h"
#include <QList>
#include <KPluginInfo>
#include <KServiceTypeTrader>

#include "kamosoplugin.h"
#include <KMessageBox>
#include <KJob>
#include <KDebug>

struct PluginManager::Private
{
	/** From singleton pattern. */
	static PluginManager* mInstance;
	
	/** list of plugin instances */
	QList<KamosoPlugin*> plugins;
	
	int runningJobs;
};

PluginManager* PluginManager::Private::mInstance=0;
PluginManager* PluginManager::self()
{
	if(!PluginManager::Private::mInstance)
		PluginManager::Private::mInstance=new PluginManager;
	return PluginManager::Private::mInstance;
}

PluginManager::PluginManager()
	: d(new Private)
{
	d->runningJobs=0;
}

PluginManager::~PluginManager()
{
	delete d;
}

KPluginInfo::List PluginManager::pluginInfo() const
{
	return KPluginInfo::fromServices(
		KServiceTypeTrader::self()->query("Kamoso/Plugin", QString()));
}

KamosoPlugin* PluginManager::loadPlugin(const KPluginInfo& pluginInfo, QObject* parent)
{
	QString error;
	KamosoPlugin* plugin=pluginInfo.service()->createInstance<KamosoPlugin>(parent, QVariantList(), &error);
	
	if(plugin)
		connect(plugin, SIGNAL(jobCreated(KJob*)), PluginManager::self(), SLOT(addJob(KJob*)));
	else
		KMessageBox::error(0, error, i18n("Error while loading the plugin '%1'", pluginInfo.name()));
	
	return plugin;
}

QList< KamosoPlugin* > PluginManager::plugins()
{
	if(d->plugins.isEmpty()) {
		//If we don't have plugins, we instanciate them all at once so that
		//we can call them at any time. All the instances stay in memory until the
		//program is unloaded (until the singleton closes.
		foreach(const KPluginInfo& pinfo, pluginInfo())
			d->plugins.append(loadPlugin(pinfo, this));
	}
	return d->plugins;
}

void PluginManager::addJob(KJob* job)
{
	kDebug() << "Job Added" << job;
	d->runningJobs++;
	connect(job, SIGNAL(result(KJob*)), SLOT(removeJob(KJob*)));
	
	emit busyState(true);
}

void PluginManager::removeJob(KJob* job)
{
	kDebug() << "Job Finnished" << job;
	
	//TODO: Notify about job errors.
	
	d->runningJobs--;
	if(d->runningJobs==0)
		emit busyState(false);
}
