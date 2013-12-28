/*  LabelModel.cpp
 *
 *  Copyright (C) 2013  Jim Evins <evins@snaught.com>
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

#include "LabelModel.h"

#include <cmath>

#include "LabelModelObject.h"
#include "LabelRegion.h"


namespace glabels
{

	///
	/// Default constructor.
	///
	LabelModel::LabelModel() : mModified(true), mTmplate(0), mRotate(false)
	{
	}


	///
	/// Add object.
	///
	void LabelModel::addObject( LabelModelObject* object )
	{
		object->setParent( this );
		mObjectList << object;

		connect( object, SIGNAL(changed()), this, SLOT(onObjectChanged()) );
		connect( object, SIGNAL(moved()), this, SLOT(onObjectMoved()) );

		mModified = true;

		emit objectAdded( object );
		emit changed();
	}


	///
	/// Object Changed Slot
	///
	void LabelModel::onObjectChanged()
	{
		mModified = true;

		emit objectChanged( qobject_cast<LabelModelObject*>(sender()) );
		emit changed();
	}


	///
	/// Object Moved Slot
	///
	void LabelModel::onObjectMoved()
	{
		mModified = true;

		emit objectMoved( qobject_cast<LabelModelObject*>(sender()) );
		emit changed();
	}


	///
	/// Delete Object
	///
	void LabelModel::deleteObject( LabelModelObject* object )
	{
		object->unselect();
		mObjectList.removeOne( object );

		disconnect( object, 0, this, 0 );

		mModified = true;

		emit objectDeleted( object );
		emit changed();
	}


	///
	/// Select Object
	///
	void LabelModel::selectObject( LabelModelObject* object )
	{
		object->select();

		emit selectionChanged();
	}


	///
	/// Unselect Object
	///
	void LabelModel::unselectObject( LabelModelObject* object )
	{
		object->unselect();

		emit selectionChanged();
	}


	///
	/// Select All Objects
	///
	void LabelModel::selectAll()
	{
		foreach ( LabelModelObject* object, mObjectList )
		{
			object->select();
		}

		emit selectionChanged();
	}


	///
	/// Unselect All Objects
	///
	void LabelModel::unselectAll()
	{
		foreach ( LabelModelObject* object, mObjectList )
		{
			object->unselect();
		}

		emit selectionChanged();
	}


	///
	/// Select Region
	///
	void LabelModel::selectRegion( const LabelRegion &region )
	{
		double rX1 = std::min( region.x1(), region.x2() );
		double rY1 = std::min( region.y1(), region.y2() );
		double rX2 = std::max( region.x1(), region.x2() );
		double rY2 = std::max( region.y1(), region.y2() );

		foreach ( LabelModelObject* object, mObjectList )
		{
			LabelRegion objectExtent = object->getExtent();

			if ( (objectExtent.x1() >= rX1) &&
			     (objectExtent.x2() <= rX2) &&
			     (objectExtent.y1() >= rY1) &&
			     (objectExtent.y2() <= rY2) )
			{
				object->select();
			}
		}

		emit selectionChanged();
	}


	///
	/// Is Selection Empty?
	///
	bool LabelModel::isSelectionEmpty()
	{
		foreach ( LabelModelObject* object, mObjectList )
		{
			if ( object->isSelected() )
			{
				return false;
			}
		}

		return true;
	}


	///
	/// Is Selection Atomic?
	///
	bool LabelModel::isSelectionAtomic()
	{
		int nSelected = 0;

		foreach ( LabelModelObject* object, mObjectList )
		{
			if ( object->isSelected() )
			{
				nSelected++;
				if ( nSelected > 1 )
				{
					return false;
				}
			}
		}

		return (nSelected == 1);
	}


	///
	/// Get List of Selected Objects
	///
	QList<LabelModelObject*> LabelModel::getSelection()
	{
		QList<LabelModelObject*> selectedList;

		foreach ( LabelModelObject* object, mObjectList )
		{
			if ( object->isSelected() )
			{
				selectedList << object;
			}
		}
		
	}


	///
	/// Get First Object in Selection List
	///
	LabelModelObject* LabelModel::getFirstSelectedObject()
	{
		foreach ( LabelModelObject* object, mObjectList )
		{
			if ( object->isSelected() )
			{
				return object;
			}
		}
	}


	///
	/// Can Any Objects in Selection Accept Text Properties?
	///
	bool LabelModel::canSelectionText()
	{
		foreach ( LabelModelObject* object, mObjectList )
		{
			if ( object->isSelected() && object->canText() )
			{
				return true;
			}
		}

		return false;
	}


	///
	/// Can Any Objects in Selection Accept Fill Property?
	///
	bool LabelModel::canSelectionFill()
	{
		foreach ( LabelModelObject* object, mObjectList )
		{
			if ( object->isSelected() && object->canFill() )
			{
				return true;
			}
		}

		return false;
	}


	///
	/// Can Any Objects in Selection Accept Line Color Property?
	///
	bool LabelModel::canSelectionLineColor()
	{
		foreach ( LabelModelObject* object, mObjectList )
		{
			if ( object->isSelected() && object->canLineColor() )
			{
				return true;
			}
		}

		return false;
	}


	///
	/// Can Any Objects in Selection Accept Line Width Property?
	///
	bool LabelModel::canSelectionLineWidth()
	{
		foreach ( LabelModelObject* object, mObjectList )
		{
			if ( object->isSelected() && object->canLineWidth() )
			{
				return true;
			}
		}

		return false;
	}


	///
	/// Delete Selected Objects
	///
	void LabelModel::deleteSelection()
	{
		QList<LabelModelObject*> selectedList = getSelection();

		foreach ( LabelModelObject* object, selectedList )
		{
			deleteObject( object );
		}

		mModified = true;

		emit changed();
		emit selectionChanged();
	}


	///
	/// Raise Selected Objects To Top
	///
	void LabelModel::raiseSelectionToTop()
	{
		QList<LabelModelObject*> selectedList = getSelection();

		foreach ( LabelModelObject* object, selectedList )
		{
			mObjectList.removeOne( object );
		}

		/// Move to end of list, representing top most object.
		foreach ( LabelModelObject* object, selectedList )
		{
			mObjectList.push_back( object );
			emit objectToTop( object );
		}

		mModified = true;

		emit changed();
	}


	///
	/// Lower Selected Objects To Bottom
	///
	void LabelModel::lowerSelectionToBottom()
	{
		QList<LabelModelObject*> selectedList = getSelection();

		foreach ( LabelModelObject* object, selectedList )
		{
			mObjectList.removeOne( object );
		}

		/// Move to front of list, representing bottom most object.
		foreach ( LabelModelObject* object, selectedList )
		{
			mObjectList.push_front( object );
			emit objectToBottom( object );
		}

		mModified = true;

		emit changed();
	}


	///
	/// Rotate Selected Objects
	///
	void LabelModel::rotateSelection( double thetaDegs )
	{
		foreach ( LabelModelObject* object, mObjectList )
		{
			if ( object->isSelected() )
			{
				object->rotate( thetaDegs );
			}
		}

		mModified = true;

		emit changed();
	}


	///
	/// Rotate Selected Objects Left 90 degrees
	///
	void LabelModel::rotateSelectionLeft()
	{
		rotateSelection( -90.0 );
	}


	///
	/// Rotate Selected Objects Right 90 degrees
	///
	void LabelModel::rotateSelectionRight()
	{
		rotateSelection( 90.0 );
	}


	///
	/// Flip Selected Objects Horizontally
	///
	void LabelModel::flipSelectionHoriz()
	{
		foreach ( LabelModelObject* object, mObjectList )
		{
			if ( object->isSelected() )
			{
				object->flipHoriz();
			}
		}

		mModified = true;

		emit changed();
	}


	///
	/// Flip Selected Objects Vertically
	///
	void LabelModel::flipSelectionVert()
	{
		foreach ( LabelModelObject* object, mObjectList )
		{
			if ( object->isSelected() )
			{
				object->flipVert();
			}
		}

		mModified = true;

		emit changed();
	}


	///
	/// Align Selected Objects To Their Left Edges
	///
	void LabelModel::alignSelectionLeft()
	{
		if ( isSelectionEmpty() || isSelectionAtomic() )
		{
			return;
		}

		QList<LabelModelObject*> selectedList = getSelection();

		/// Find left-most edge.
		double x1_min = 7200; /// Start with a very large value: 7200pts = 100in
		foreach ( LabelModelObject* object, selectedList )
		{
			LabelRegion r = object->getExtent();
			if ( r.x1() < x1_min ) x1_min = r.x1();
		}

		/// Now adjust the object positions to line up the left edges at left-most edge.
		foreach ( LabelModelObject* object, selectedList )
		{
			LabelRegion r = object->getExtent();
			double dx = x1_min - r.x1();
			object->setPositionRelative( dx, 0 );
		}
		
		mModified = true;

		emit changed();
	}


	///
	/// Align Selected Objects To Their Right Edges
	///
	void LabelModel::alignSelectionRight()
	{
		if ( isSelectionEmpty() || isSelectionAtomic() )
		{
			return;
		}

		QList<LabelModelObject*> selectedList = getSelection();

		/// Find right-most edge.
		double x1_max = -7200; /// Start with a very large negative value: 7200pts = 100in
		foreach ( LabelModelObject* object, selectedList )
		{
			LabelRegion r = object->getExtent();
			if ( r.x1() > x1_max ) x1_max = r.x1();
		}

		/// Now adjust the object positions to line up the right edges at right-most edge.
		foreach ( LabelModelObject* object, selectedList )
		{
			LabelRegion r = object->getExtent();
			double dx = x1_max - r.x1();
			object->setPositionRelative( dx, 0 );
		}
		
		mModified = true;

		emit changed();
	}


	///
	/// Align Selected Objects To Their Horizontal Centers
	///
	void LabelModel::alignSelectionHCenter()
	{
		if ( isSelectionEmpty() || isSelectionAtomic() )
		{
			return;
		}

		QList<LabelModelObject*> selectedList = getSelection();

		/// Find average center of objects.
		double xsum = 0;
		int n = 0;
		foreach ( LabelModelObject* object, selectedList )
		{
			LabelRegion r = object->getExtent();
			xsum += (r.x1() + r.x2()) / 2.0;
			n++;
		}
		double xavg = xsum / n;

		/// Find object closest to average center of objects.
		double xcenter = 7200; /// Start with very large value.
		double dxmin = fabs( xavg - xcenter );
		foreach ( LabelModelObject* object, selectedList )
		{
			LabelRegion r = object->getExtent();
			double dx = fabs( xavg - (r.x1() + r.x2())/2.0 );
			if ( dx < dxmin )
			{
				dxmin = dx;
				xcenter = (r.x1() + r.x2()) / 2.0;
			}
		}

		/// Now adjust the object positions to line up with the center of this object.
		foreach ( LabelModelObject* object, selectedList )
		{
			LabelRegion r = object->getExtent();
			double dx = xcenter - (r.x1() + r.x2())/2.0;
			object->setPositionRelative( dx, 0 );
		}
		
		mModified = true;

		emit changed();
	}


	///
	/// Align Selected Objects To Their Top Edges
	///
	void LabelModel::alignSelectionTop()
	{
		if ( isSelectionEmpty() || isSelectionAtomic() )
		{
			return;
		}

		QList<LabelModelObject*> selectedList = getSelection();

		/// Find top-most edge.
		double y1_min = 7200; /// Start with a very large value: 7200pts = 100in
		foreach ( LabelModelObject* object, selectedList )
		{
			LabelRegion r = object->getExtent();
			if ( r.y1() < y1_min ) y1_min = r.y1();
		}

		/// Now adjust the object positions to line up the top edges at top-most edge.
		foreach ( LabelModelObject* object, selectedList )
		{
			LabelRegion r = object->getExtent();
			double dy = y1_min - r.y1();
			object->setPositionRelative( 0, dy );
		}
		
		mModified = true;

		emit changed();
	}


	///
	/// Align Selected Objects To Their Bottom Edges
	///
	void LabelModel::alignSelectionBottom()
	{
		if ( isSelectionEmpty() || isSelectionAtomic() )
		{
			return;
		}

		QList<LabelModelObject*> selectedList = getSelection();

		/// Find bottom-most edge.
		double y1_max = -7200; /// Start with a very large negative value: 7200pts = 100in
		foreach ( LabelModelObject* object, selectedList )
		{
			LabelRegion r = object->getExtent();
			if ( r.y1() > y1_max ) y1_max = r.y1();
		}

		/// Now adjust the object positions to line up the bottom edges at bottom-most edge.
		foreach ( LabelModelObject* object, selectedList )
		{
			LabelRegion r = object->getExtent();
			double dy = y1_max - r.y1();
			object->setPositionRelative( 0, dy );
		}
		
		mModified = true;

		emit changed();
	}


	///
	/// Align Selected Objects To Their Vertical Centers Edges
	///
	void LabelModel::alignSelectionVCenter()
	{
		if ( isSelectionEmpty() || isSelectionAtomic() )
		{
			return;
		}

		QList<LabelModelObject*> selectedList = getSelection();

		/// Find average center of objects.
		double ysum = 0;
		int n = 0;
		foreach ( LabelModelObject* object, selectedList )
		{
			LabelRegion r = object->getExtent();
			ysum += (r.y1() + r.y2()) / 2.0;
			n++;
		}
		double yavg = ysum / n;

		/// Find object closest to average center of objects.
		double ycenter = 7200; /// Start with very large value.
		double dymin = fabs( yavg - ycenter );
		foreach ( LabelModelObject* object, selectedList )
		{
			LabelRegion r = object->getExtent();
			double dy = fabs( yavg - (r.y1() + r.y2())/2.0 );
			if ( dy < dymin )
			{
				dymin = dy;
				ycenter = (r.y1() + r.y2()) / 2.0;
			}
		}

		/// Now adjust the object positions to line up with the center of this object.
		foreach ( LabelModelObject* object, selectedList )
		{
			LabelRegion r = object->getExtent();
			double dy = ycenter - (r.y1() + r.y2())/2.0;
			object->setPositionRelative( 0, dy );
		}
		
		mModified = true;

		emit changed();
	}


	///
	/// Align Selected Objects To Center Of Label Horizontally
	///
	void LabelModel::centerSelectionHoriz()
	{
		double xLabelCenter = w() / 2.0;

		foreach ( LabelModelObject* object, mObjectList )
		{
			if ( object->isSelected() )
			{
				LabelRegion r = object->getExtent();
				double xObjectCenter = (r.x1() + r.x2()) / 2.0;
				double dx = xLabelCenter - xObjectCenter;
				object->setPositionRelative( dx, 0 );
			}
		}

		mModified = true;

		emit changed();
	}


	///
	/// Align Selected Objects To Center Of Label Vertically
	///
	void LabelModel::centerSelectionVert()
	{
		double yLabelCenter = h() / 2.0;

		foreach ( LabelModelObject* object, mObjectList )
		{
			if ( object->isSelected() )
			{
				LabelRegion r = object->getExtent();
				double yObjectCenter = (r.y1() + r.y2()) / 2.0;
				double dy = yLabelCenter - yObjectCenter;
				object->setPositionRelative( 0, dy );
			}
		}

		mModified = true;

		emit changed();
	}


	///
	/// Move Selected Objects By dx,dy
	///
	void LabelModel::moveSelection( double dx, double dy )
	{
		foreach ( LabelModelObject* object, mObjectList )
		{
			if ( object->isSelected() )
			{
				object->setPositionRelative( dx, dy );
			}
		}

		mModified = true;

		emit changed();
	}


	///
	/// Set Font Family Of Selected Objects
	///
	void LabelModel::setSelectionFontFamily( const QString &fontFamily )
	{
		foreach ( LabelModelObject* object, mObjectList )
		{
			if ( object->isSelected() )
			{
				object->setFontFamily( fontFamily );
			}
		}

		mModified = true;

		emit changed();
	}


	///
	/// Set Font Size Of Selected Objects
	///
	void LabelModel::setSelectionFontSize( double fontSize )
	{
		foreach ( LabelModelObject* object, mObjectList )
		{
			if ( object->isSelected() )
			{
				object->setFontSize( fontSize );
			}
		}

		mModified = true;

		emit changed();
	}


	///
	/// Set Font Weight Of Selected Objects
	///
	void LabelModel::setSelectionFontWeight( QFont::Weight fontWeight )
	{
		foreach ( LabelModelObject* object, mObjectList )
		{
			if ( object->isSelected() )
			{
				object->setFontWeight( fontWeight );
			}
		}

		mModified = true;

		emit changed();
	}


	///
	/// Set Font Italic Flag Of Selected Objects
	///
	void LabelModel::setSelectionFontItalicFlag( bool fontItalicFlag )
	{
		foreach ( LabelModelObject* object, mObjectList )
		{
			if ( object->isSelected() )
			{
				object->setFontItalicFlag( fontItalicFlag );
			}
		}

		mModified = true;

		emit changed();
	}


	///
	/// Set Text Horizontal Alignment Of Selected Objects
	///
	void LabelModel::setSelectionTextHAlign( Qt::Alignment textHAlign )
	{
		foreach ( LabelModelObject* object, mObjectList )
		{
			if ( object->isSelected() )
			{
				object->setTextHAlign( textHAlign );
			}
		}

		mModified = true;

		emit changed();
	}


	///
	/// Set Text Vertical Alignment Of Selected Objects
	///
	void LabelModel::setSelectionTextVAlign( Qt::Alignment textVAlign )
	{
		foreach ( LabelModelObject* object, mObjectList )
		{
			if ( object->isSelected() )
			{
				object->setTextVAlign( textVAlign );
			}
		}

		mModified = true;

		emit changed();
	}


	///
	/// Set Text Line Spacing Of Selected Objects
	///
	void LabelModel::setSelectionTextLineSpacing( double textLineSpacing )
	{
		foreach ( LabelModelObject* object, mObjectList )
		{
			if ( object->isSelected() )
			{
				object->setTextLineSpacing( textLineSpacing );
			}
		}

		mModified = true;

		emit changed();
	}


	///
	/// Set Text Color Node Of Selected Objects
	///
	void LabelModel::setSelectionTextColorNode( ColorNode textColorNode )
	{
		foreach ( LabelModelObject* object, mObjectList )
		{
			if ( object->isSelected() )
			{
				object->setTextColorNode( textColorNode );
			}
		}

		mModified = true;

		emit changed();
	}


	///
	/// Set Line Width Of Selected Objects
	///
	void LabelModel::setSelectionLineWidth( double lineWidth )
	{
		foreach ( LabelModelObject* object, mObjectList )
		{
			if ( object->isSelected() )
			{
				object->setLineWidth( lineWidth );
			}
		}

		mModified = true;

		emit changed();
	}


	///
	/// Set Line Color Node Of Selected Objects
	///
	void LabelModel::setSelectionLineColorNode( ColorNode lineColorNode )
	{
		foreach ( LabelModelObject* object, mObjectList )
		{
			if ( object->isSelected() )
			{
				object->setLineColorNode( lineColorNode );
			}
		}

		mModified = true;

		emit changed();
	}


	///
	/// Set Fill Color Node Of Selected Objects
	///
	void LabelModel::setSelectionFillColorNode( ColorNode fillColorNode )
	{
		foreach ( LabelModelObject* object, mObjectList )
		{
			if ( object->isSelected() )
			{
				object->setFillColorNode( fillColorNode );
			}
		}

		mModified = true;

		emit changed();
	}

}

