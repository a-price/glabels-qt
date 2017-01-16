/*  Db.cpp
 *
 *  Copyright (C) 2013-2016  Jim Evins <evins@snaught.com>
 *
 *  This file is part of gLabels-qt.
 *
 *  gLabels-qt is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  gLabels-qt is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with gLabels-qt.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "Db.h"


#include <QApplication>
#include <QtDebug>

#include "Config.h"
#include "StrUtil.h"
#include "XmlCategoryParser.h"
#include "XmlPaperParser.h"
#include "XmlTemplateParser.h"
#include "XmlVendorParser.h"


namespace glabels
{
	
	//
	// Private
	//
	namespace
	{
		const QString    empty = "";
	
		bool partNameLessThan( const Template *a, const Template *b )
		{
			return StrUtil::comparePartNames( a->name(), b->name() ) < 0;
		}
	}


	//
	// Static data
	//
	QList<Paper*>    Db::mPapers;
	QStringList      Db::mPaperIds;
	QStringList      Db::mPaperNames;
	QList<Category*> Db::mCategories;
	QStringList      Db::mCategoryIds;
	QStringList      Db::mCategoryNames;
	QList<Vendor*>   Db::mVendors;
	QStringList      Db::mVendorNames;
	QList<Template*> Db::mTemplates;
	QString          Db::mPaperNameOther;

	
	Db::Db()
	{
		mPaperNameOther = tr("Other");

		readPapers();
		readCategories();
		readVendors();
		readTemplates();
	}


	void Db::init()
	{
		instance();
	}

	
	Db* Db::instance()
	{
		static Db* db = new Db();
		return db;
	}


	const QList<Paper*>& Db::papers()
	{
		return mPapers;
	}


	const QStringList& Db::paperIds()
	{
		return mPaperIds;
	}


	const QStringList& Db::paperNames()
	{
		return mPaperNames;
	}


	const QList<Category*>& Db::categories()
	{
		return mCategories;
	}


	const QStringList& Db::categoryIds()
	{
		return mCategoryIds;
	}


	const QStringList& Db::categoryNames()
	{
		return mCategoryNames;
	}


	const QList<Vendor*>& Db::vendors()
	{
		return mVendors;
	}


	const QStringList& Db::vendorNames()
	{
		return mVendorNames;
	}


	const QList<Template*>& Db::templates()
	{
		return mTemplates;
	}


	void Db::registerPaper( Paper *paper )
	{
		if ( !isPaperIdKnown( paper->id() ) )
		{
			mPapers << paper;
			mPaperIds << paper->id();
			mPaperNames << paper->name();
		}
		else
		{
			qWarning() << "Duplicate paper ID: " << paper->id();
		}
	}


	const Paper *Db::lookupPaperFromName( const QString& name )
	{
		if ( name.isNull() || name.isEmpty() )
		{
			qWarning() << "NULL paper name.";
			return mPapers.first();
		}

		foreach ( Paper *paper, mPapers )
		{
			if ( paper->name() == name )
			{
				return paper;
			}
		}

		qWarning() << "Unknown paper name: " << name;
		return NULL;
	}


	const Paper *Db::lookupPaperFromId( const QString& id )
	{
		if ( id.isNull() || id.isEmpty() )
		{
			qWarning() << "NULL paper ID.";
			return mPapers.first();
		}

		foreach ( Paper *paper, mPapers )
		{
			if ( paper->id() == id )
			{
				return paper;
			}
		}

		qWarning() << "Unknown paper ID: " << id;
		return NULL;
	}


	QString Db::lookupPaperIdFromName( const QString& name )
	{
		if ( !name.isNull() && !name.isEmpty() )
		{
			const Paper *paper = lookupPaperFromName( name );
			if ( paper != NULL )
			{
				return paper->id();
			}
		}

		qWarning() << "Unknown paper name: " << name;
		return empty;
	}


	QString Db::lookupPaperNameFromId( const QString& id )
	{
		if ( !id.isNull() && !id.isEmpty() )
		{
			if ( isPaperIdOther( id ) )
			{
				return mPaperNameOther;
			}

			const Paper *paper = lookupPaperFromId( id );
			if ( paper != NULL )
			{
				return paper->name();
			}
		}

		qWarning() << "Unknown paper id: " << id;
		return empty;
	}


	bool Db::isPaperIdKnown( const QString& id )
	{
		foreach ( Paper *paper, mPapers )
		{
			if ( paper->id() == id )
			{
				return true;
			}
		}

		return false;
	}


	bool Db::isPaperIdOther( const QString& id )
	{
		return ( id == "Other" );
	}


	void Db::registerCategory( Category *category )
	{
		if ( !isCategoryIdKnown( category->id() ) )
		{
			mCategories << category;
			mCategoryIds << category->id();
			mCategoryNames << category->name();
		}
		else
		{
			qWarning() << "Duplicate category ID: " << category->id();
		}
	}


	const Category *Db::lookupCategoryFromName( const QString& name )
	{
		if ( name.isNull() || name.isEmpty() )
		{
			qWarning() << "NULL category name.";
			return mCategories.first();
		}

		foreach ( Category *category, mCategories )
		{
			if ( category->name() == name )
			{
				return category;
			}
		}

		qWarning() << "Unknown category name: \"%s\"." << name;
		return NULL;
	}


	const Category *Db::lookupCategoryFromId( const QString& id )
	{
		if ( id.isNull() || id.isEmpty() )
		{
			qDebug() << "NULL category ID.";
			return mCategories.first();
		}

		foreach ( Category *category, mCategories )
		{
			if ( category->id() == id )
			{
				return category;
			}
		}

		qWarning() << "Unknown category ID: " << id;
		return NULL;
	}


	QString Db::lookupCategoryIdFromName( const QString& name )
	{
		if ( !name.isNull() && !name.isEmpty() )
		{
			const Category *category = lookupCategoryFromName( name );
			if ( category != NULL )
			{
				return category->id();
			}
		}

		qWarning() << "Unknown category name: " << name;
		return empty;
	}


	QString Db::lookupCategoryNameFromId( const QString& id )
	{
		if ( !id.isNull() && !id.isEmpty() )
		{
			const Category *category = lookupCategoryFromId( id );
			if ( category != NULL )
			{
				return category->name();
			}
		}

		qWarning() << "Unknown category id: " << id;
		return empty;
	}


	bool Db::isCategoryIdKnown( const QString& id )
	{
		foreach ( Category *category, mCategories )
		{
			if ( category->id() == id )
			{
				return true;
			}
		}

		return false;
	}


	void Db::registerVendor( Vendor *vendor )
	{
		if ( !isVendorNameKnown( vendor->name() ) )
		{
			mVendors << vendor;
			mVendorNames << vendor->name();
		}
		else
		{
			qWarning() << "Duplicate vendor name: " << vendor->name();
		}
	}


	const Vendor *Db::lookupVendorFromName( const QString& name )
	{
		if ( name.isNull() || name.isEmpty() )
		{
			qWarning() << "NULL vendor name.";
			return mVendors.first();
		}

		foreach ( Vendor *vendor, mVendors )
		{
			if ( vendor->name() == name )
			{
				return vendor;
			}
		}

		qWarning() << "Unknown vendor name: " << name;
		return NULL;
	}


	QString Db::lookupVendorUrlFromName( const QString& name )
	{
		if ( !name.isNull() && !name.isEmpty() )
		{
			const Vendor *vendor = lookupVendorFromName( name );
			if ( vendor != NULL )
			{
				return vendor->url();
			}
		}

		qWarning() << "Unknown vendor name: " << name;
		return empty;
	}


	bool Db::isVendorNameKnown( const QString& name )
	{
		foreach ( Vendor *vendor, mVendors )
		{
			if ( vendor->name() == name )
			{
				return true;
			}
		}

		return false;
	}


	void Db::registerTemplate( Template *tmplate )
	{
		if ( !isTemplateKnown( tmplate->brand(), tmplate->part() ) )
		{
			tmplate->initPreview();
			mTemplates << tmplate;
		}
		else
		{
			qWarning() << "Duplicate template name: " << tmplate->name();
		}
	}


	const Template *Db::lookupTemplateFromName( const QString& name )
	{
		if ( name.isNull() || name.isEmpty() )
		{
			qWarning() << "NULL template name.";
			return mTemplates.first();
		}

		foreach ( Template *tmplate, mTemplates )
		{
			if ( tmplate->name() == name )
			{
				return tmplate;
			}
		}

		qWarning() << "Unknown template name: " << name;
		return NULL;
	}


	const Template *Db::lookupTemplateFromBrandPart( const QString& brand, const QString& part )
	{
		if ( brand.isNull() || brand.isEmpty() || part.isNull() || part.isEmpty() )
		{
			qWarning() << "NULL template brand and/or part.";
			return mTemplates.first();
		}

		foreach ( Template *tmplate, mTemplates )
		{
			if ( (tmplate->brand() == brand) && (tmplate->part() == part) )
			{
				return tmplate;
			}
		}

		qWarning() << "Unknown template brand, part: " << brand << ", " << part;
		return NULL;
	}


	bool Db::isTemplateKnown( const QString& brand, const QString& part )
	{
		foreach ( Template *tmplate, mTemplates )
		{
			if ( (tmplate->brand() == brand) && (tmplate->part() == part) )
			{
				return true;
			}
		}

		return false;
	}


	QStringList Db::getNameListOfSimilarTemplates( const QString& name )
	{
		QStringList list;

		const Template *tmplate1 = lookupTemplateFromName( name );
		if ( tmplate1 == NULL )
		{
			qWarning() << "Unknown template name: " << name;
			return list;
		}

		foreach (const Template *tmplate2, mTemplates )
		{
			if ( tmplate1->name() != tmplate2->name() )
			{
				if ( tmplate1->isSimilarTo( tmplate2 ) )
				{
					list << tmplate2->name();
				}
			}
		}

		return list;
	}


	void Db::registerUserTemplate( Template *templat )
	{
		// TODO
	}


	void Db::deleteUserTemplateByName( const QString& name )
	{
		// TODO
	}


	void Db::deleteUserTemplateByBrandPart( const QString& brand, const QString& part )
	{
		// TODO
	}


	void Db::printKnownPapers()
	{
		qDebug() << "KNOWN PAPERS:";

		foreach ( Paper *paper, mPapers )
		{
			qDebug() << "paper "
				 << "id="       << paper->id()          << ", "
				 << "name="     << paper->name()        << ", "
				 << "width="    << paper->width().pt()  << "pts, "
				 << "height="   << paper->height().pt() << "pts, "
				 << "pwg_size=" << paper->pwgSize();
		}

		qDebug();
	}


	void Db::printKnownCategories()
	{
		qDebug() << "KNOWN CATEGORIES:";

		foreach ( Category *category, mCategories )
		{
			qDebug() << "category "
				 << "id="    << category->id()   << ", "
				 << "name="  << category->name();
		}

		qDebug();
	}


	void Db::printKnownVendors()
	{
		qDebug() << "KNOWN VENDORS:";

		foreach ( Vendor *vendor, mVendors )
		{
			qDebug() << "vendor "
				 << "name='" << vendor->name() << ", "
				 << "url='"  << vendor->url();
		}

		qDebug();
	}


	void Db::printKnownTemplates()
	{
		qDebug() << "KNOWN TEMPLATES:";

		foreach ( Template *tmplate, mTemplates )
		{
			qDebug() << "template "
				 << "brand="       << tmplate->brand()       << ", "
				 << "part="        << tmplate->part()        << ", "
				 << "description=" << tmplate->description();
		}

		qDebug();
	}


	QDir Db::systemTemplatesDir()
	{
		QDir dir(QApplication::applicationDirPath());

		if ( dir.dirName() == "bin" )
		{
			dir.cdUp();
			dir.cd( "share" );
			dir.cd( "libglabels-3.0" ); // TODO: install qt version
		}
		else
		{
			// Working out of build directory
			dir.cd( Config::PROJECT_SOURCE_DIR );
		}

		dir.cd( "templates" );
		return dir;
	}


	void Db::readPapers()
	{
		readPapersFromDir( systemTemplatesDir() );
	}


	void Db::readPapersFromDir( const QDir& dir )
	{
		XmlPaperParser parser;

		foreach ( QString fileName, dir.entryList( QDir::Files ) )
		{
			if ( fileName == "paper-sizes.xml" )
			{
				parser.readFile( dir.absoluteFilePath( fileName ) );
			}
		}
	}


	void Db::readCategories()
	{
		readCategoriesFromDir( systemTemplatesDir() );
	}


	void Db::readCategoriesFromDir( const QDir& dir )
	{
		XmlCategoryParser parser;

		foreach ( QString fileName, dir.entryList( QDir::Files ) )
		{
			if ( fileName == "categories.xml" )
			{
				parser.readFile( dir.absoluteFilePath( fileName ) );
			}
		}
	}


	void Db::readVendors()
	{
		readVendorsFromDir( systemTemplatesDir() );
	}


	void Db::readVendorsFromDir( const QDir& dir )
	{
		XmlVendorParser parser;

		foreach ( QString fileName, dir.entryList( QDir::Files ) )
		{
			if ( fileName == "vendors.xml" )
			{
				parser.readFile( dir.absoluteFilePath( fileName ) );
			}
		}
	}


	void Db::readTemplates()
	{
		readTemplatesFromDir( systemTemplatesDir() );

		// TODO: Read user directories

		qStableSort( mTemplates.begin(), mTemplates.end(), partNameLessThan );
	}


	void Db::readTemplatesFromDir( const QDir& dir )
	{
		QStringList filters;
		filters << "*-templates.xml" << "*.template";

		XmlTemplateParser parser;

		foreach ( QString fileName, dir.entryList( filters, QDir::Files ) )
		{
			parser.readFile( dir.absoluteFilePath( fileName ) );
		}
	}

}
