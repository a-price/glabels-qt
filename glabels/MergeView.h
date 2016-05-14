/*  MergeView.h
 *
 *  Copyright (C) 2016  Jim Evins <evins@snaught.com>
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

#ifndef MergeView_h
#define MergeView_h

#include "ui_MergeView.h"


// Forward references
class LabelModel;
class UndoRedoModel;
class Merge;
	

///
/// Merge Property Editor Widget
///
class MergeView : public QWidget, public Ui_MergeView
{
	Q_OBJECT


	/////////////////////////////////
	// Life Cycle
	/////////////////////////////////
public:
	MergeView( QWidget *parent = 0 );
	~MergeView();


	/////////////////////////////////
	// Public methods
	/////////////////////////////////
	void setModel( LabelModel* model, UndoRedoModel* undoRedoModel );


	/////////////////////////////////
	// Slots
	/////////////////////////////////
private slots:
	void onMergeChanged();
	void onMergeSourceChanged();
	void onMergeSelectionChanged();

	void onFormatComboActivated();
	void onLocationButtonClicked();
	void onSelectAllButtonClicked();
	void onUnselectAllButtonClicked();
	void onCellChanged( int iRow, int iCol );


	/////////////////////////////////
	// Private methods
	/////////////////////////////////
private:
	void loadHeaders( Merge* merge );
	void loadTable( Merge* merge );


	/////////////////////////////////
	// Private Data
	/////////////////////////////////
private:
	QStringList  mMergeFormatNames;
	
	LabelModel*    mModel;
	UndoRedoModel* mUndoRedoModel;

	QStringList mKeys;
	QString     mPrimaryKey;

	QString mCwd;

	bool mBlock;
	int  mOldFormatComboIndex;

};


#endif // MergeView_h
