/*************************************************************************************
 *  Copyright (C) 2009 by Aleix Pol <aleixpol@kde.org>                               *
 *                                                                                   *
 *  This program is free software; you can redistribute it and/or                    *
 *  modify it under the terms of the GNU General Public License                      *
 *  as published by the Free Software Foundation; either version 3                   *
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

#include "facebook.h"
#include <KPluginFactory>
#include <KAboutData>
#include <KMimeType>
#include <KDebug>
#include <QAction>

K_PLUGIN_FACTORY(KDevExecuteFactory, registerPlugin<FacebookPlugin>(); )
K_EXPORT_PLUGIN(KDevExecuteFactory(KAboutData("facebooksender", "facebooksender", ki18n("Facebook support"), "0.1", ki18n("Allows to communicate with Facebook"), KAboutData::License_GPL)))

FacebookPlugin::FacebookPlugin(QObject* parent, const QVariantList& args)
	: KamosoPlugin(parent, args)
{}

QAction* FacebookPlugin::thumbnailsAction(const KUrl& url)
{
	KMimeType::Ptr mime = KMimeType::findByUrl(url);
	QAction* act=0;
	mSelectedUrls.clear();
	if(mime->name().startsWith("image/")) {
		act=new QAction(i18n("Upload to Facebook..."), 0);
		connect(act, SIGNAL(triggered(bool)), SLOT(uploadImage(bool)));
		
		mSelectedUrls=url;
	}
	return act;
}

void FacebookPlugin::uploadImage(bool )
{
	Q_ASSERT(!mSelectedUrls.isEmpty());
	kDebug() << "uploading..." << mSelectedUrls;
}
