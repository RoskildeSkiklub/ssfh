//
// Snow Stuff For Hire - A POS-like system designed for very quick rental
// operations at small ski resorts
// Copyright (C) 2014 Mads Bondo Dydensborg <mads@dydensborg.dk>
//
// This file is part of Snow Stuff For Hire.
//
// Snow Stuff For Hire is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Snow Stuff For Hire is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Snow Stuff For Hire.  If not, see <http://www.gnu.org/licenses/>.
//

#include "reiddialog.h"
#include "ui_reiddialog.h"

// Qt
#include <QAbstractButton>
#include <QMessageBox>
#include <QRegExp>
#include <QValidator>
#include <QInputDialog>
#include <QFileDialog>
#include <QSet>
#include <QList>
#include <QPair>

// app
#include "log.h"
#include "globals.h"
#include "exception.h"
#include "utility.h"
#include "item.h"

using namespace Log;

ReIdDialog::ReIdDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ReIdDialog)
{
    ui->setupUi(this);

    // Set validators for to_id lineedit.
    // Technically, one could envision ids different from only numbers...
    QRegExp rx( "\\d+" );
    QValidator *validator = new QRegExpValidator( rx, this);
    ui->input_toId_lineEdit->setValidator( validator );

    // Clear the fields, set enabled on buttons, etc.
    clear();

    // Get the signals from the scan input events.
    connect( Globals::interceptor, SIGNAL(barcodeItemScan(QString)),
             this, SLOT(scan_item(QString) ) );
    //! \todo Provide audio feedback when returning/adding items
    connect(this, SIGNAL(item_scanned()),
            Globals::getFeedbackObject(), SLOT(itemScanned()));
}

void ReIdDialog::clear() {
    Logger log( "void ReIdDialog::clear()" );
    //! \todo Fix up

    // Clear and enable fromId lineEdit
    ui->input_fromId_lineEdit->setText( "" );
    ui->input_fromId_lineEdit->setReadOnly( false );

    // Lookup is not enabled, fromId is blank
    ui->input_lookup_pushButton->setEnabled( false );

    // ToId not enabled, until lookup has been performed
    ui->input_toId_lineEdit->setText( "");
    ui->input_toId_lineEdit->setEnabled( false );

    // Clear the item description
    ui->output_contract_textBrowser->setText( "" );

    // Disable accept/ok button
    ui->buttonBox->button( QDialogButtonBox::Ok )->setEnabled( false );
    ui->buttonBox->button( QDialogButtonBox::Apply )->setEnabled( false );

    // Set focus to fromId
    ui->input_fromId_lineEdit->setFocus();
}

ReIdDialog::~ReIdDialog()
{
    delete ui;
}

void ReIdDialog::scan_item(const QString &item_id) {
    Logger log("ReidDialog::scan_item(const QString &item_id)");
    // Use readonly status of fromId to determine where we are...

    if ( ui->input_fromId_lineEdit->isReadOnly() ) {
        log.stream() << "fromId is readonly, inputting stuff into toId";
        // Insert into rentItem and do the swap.
        ui->input_toId_lineEdit->setText( item_id );
        //! \todo
        doReId();
        // BUG: Probably wrong to emit after
        emit item_scanned();
    } else {
        log.stream() << "fromId is writeable, inputting stuff into fromId";
        // Into returnItem, and try to return
        ui->input_fromId_lineEdit->setText( item_id );
        //! \todo
        fromIdSet();
        // BUG: Probably wrong to emit after
        emit item_scanned();
    }
}

void ReIdDialog::fromIdSet() {
    Logger log("ReidDialog::fromIdSet()");
    if ( ui->input_fromId_lineEdit->text().isEmpty() ) {
        throw Exception( Errors::InternalError )
                << ( log.stream( error )
                     << "ReidDialog::fromIdSet called with empty fromId" );
    }
    // Locate item
    QString fromId = ui->input_fromId_lineEdit->text();
    Item item = Item::db_load( fromId );
    //! \todo - nice error, if item does not exist..
    log.stream() << "Item with id '" << item.getId() << "' loaded from database";
    ui->output_contract_textBrowser->setText( item.toHtml() );
    // Lock the field
    ui->input_fromId_lineEdit->setReadOnly( true );
    ui->input_lookup_pushButton->setEnabled( false );
    ui->input_toId_lineEdit->setEnabled( true );
    ui->input_toId_lineEdit->setFocus();
    //! \todo
    // ui->buttonBox->button( QDialogButtonBox::Ok )->setEnabled( ui->input_returnItem_lineEdit->isReadOnly() && !ui->input_rentItem_lineEdit->text().isEmpty() );
}

bool ReIdDialog::doReId() {
    Logger log( "void ReidDialog::doReId()" );
    QString from_id = ui->input_fromId_lineEdit->text();
    QString to_id = ui->input_toId_lineEdit->text();
    try {
        Item::db_reid( from_id, to_id );
    }
    catch ( const Exception & ex ) {
        log.stream( debug ) << "Item::db_reid failed, checking reason";
        switch ( ex.getStatusCode() ) {
        case Errors::ItemDoesNotExist: {
            QMessageBox::critical( this, tr( "Item does not exist"),
                                  tr( "The item with id '%1' was not found in the database, so it can not be given the new id '%2'.").arg( from_id ).arg( to_id ) );
            return false;
        }
        case Errors::ItemAlreadyExists: {
            QMessageBox::critical( this, tr( "Item already exist"),
                                  tr( "The item with id '%1' was found in the database, so the item with id '%2' can not be given the new id '%1'.").arg( to_id ).arg( from_id ) );
            return false;
        }
        default: {
            log.stream( error ) << "Unexpected exception from Item::db_reid : " << ex.toString();
            throw;
        }
        }
    }
    QMessageBox::information( this, tr( "Item has been given a new id"),
                             tr( "The item with id '%1' now have id '%2'").arg( from_id).arg( to_id ) );
    clear();
    return true;
}

void ReIdDialog::readChangesFromFile() {
    Logger log("void ReIdDialog::readChangesFromFile()");
    // Show information about the process to the user
    QMessageBox::StandardButton res
            = QMessageBox::information( this, tr( "Reading id changes from a file"),
                                       tr( "<p>In order to read id changes from a file, you will have to provide a filename.</p><p>The file will be assumed to contain one item id change pr. line. The default format for each line is <tt>id=>id</tt>, but this can be changed by providing your own pattern.</p><p>Note, that the file can not change the same item twice. This will be checked prior to any changes occuring.</p>"),  QMessageBox::Ok | QMessageBox::Cancel );
    if ( res == QMessageBox::Cancel ) {
        log.stream() << "User cancelled information box, stopping";
        return;
    }

    // Select a file
    log.stream() << "Asking the user about a filename";
    QString fileName
            = QFileDialog::getOpenFileName( this, tr( "Select file with item id changes") );
    if ( fileName.isEmpty() ) {
        log.stream() << "User cancelled file dialog, stopping";
        return;
    }

    // Make sure we can open the file
    QFile file( fileName );
    if ( !file.open(QIODevice::ReadOnly | QIODevice::Text) ) {
        QMessageBox::critical( this, tr( "Unable to read from file"),
                              tr( "Unable to read from file '%1'. Aborting.").arg( fileName ) );
        log.stream( warn ) << "Was unable to read from userselected file " << fileName << " during reid from file";
        return;
    }

    // Get a regexp pattern
    log.stream() << "Asking user about a regexp pattern";
    bool ok;
    QString parseReg = "(\\d+)=>(\\d+)";
    parseReg = QInputDialog::getText( this, tr( "Input pattern for lines"),
                                     tr( "Format for lines (id=>id)" ),
                                     QLineEdit::Normal,
                                     parseReg, &ok );
    if ( !ok ) {
        log.stream() << "User cancelled pattern dialog, stopping";
        return;
    }
    QRegExp re( parseReg );

    // Parse the file and make sure that all lines are readable
    log.stream() << "Parsing the file";
    // Check that no dups are present
    QSet<QString> dup_set;
    // Save mappings for later
    QList< QPair< QString,QString> > reid_list;
    QString from_id;
    QString to_id;
    QString dup_id;
    QTextStream in( &file );
    int count = 0;
    while ( !in.atEnd() ) {
        QString line = in.readLine();
        ++count;
        if ( ! re.exactMatch( line ) ) {
            QMessageBox::critical( this, tr( "Unable to extract ids from line"),
                                  tr( "<p>The following line</p><p><tt>%1: %2</tt></p><p>did not match the pattern '%3'. No changes have been committed. Aborting." ).arg( count ).arg( line ).arg( parseReg ) );
            log.stream(error) << "The line '" << line << "' did not match the pattern '"
                              << parseReg << "'. No changes committed, aborting";
            return;
        }
        from_id = re.cap( 1 );
        to_id = re.cap( 2 );
        if ( dup_set.contains( to_id ) ) {
            dup_id = to_id;
        }
        if ( dup_set.contains( from_id ) ) {
            dup_id = from_id;
        }
        if ( !dup_id.isEmpty() ) {
            QMessageBox::critical( this, tr( "Duplicate item id change found" ),
                                  tr( "The id '%1' was present in the file more than once. This is not allowed. No changes committed. Aborting." )
                                  .arg( dup_id ) );
            log.stream( warn ) << "File with id changes contained the item id '" << dup_id << "' more than once. No changes committed. Aborting.";
            return;
        }
        // Check that from_id exists, and that to_id does not.
        if ( !Item::db_exists( from_id ) ) {
            QMessageBox::critical( this, tr( "Item does not exist" ),
                                  tr( "<p>The item with id '%1' does not exist in the database, but this line indicated a id change from this id.</p><p><tt>%2: %3</tt></p><p>No changes committed. Aborting.</p>" )
                                  .arg( from_id ).arg( count ).arg( line ) );
            log.stream( warn ) << "File with id changed contained the item id '" << from_id
                                  << "' as from_id but that item does not exist. No changes committed, aborting";
            return;
        }
        if ( Item::db_exists( to_id ) ) {
            QMessageBox::critical( this, tr( "Item already exist" ),
                                  tr( "<p>The item with id '%1' already exist in the database, but this line indicated an id change to this id.</p><p><tt>%2: %3</tt></p><p>No changes committed. Aborting.</p>" )
                                  .arg( to_id ).arg( count ).arg( line ) );
            log.stream( warn ) << "File with id changed contained the item id '" << from_id
                                  << "' as to_id but that item already exists. No changes committed, aborting";
            return;
        }
        dup_set.insert( from_id );
        dup_set.insert( to_id );
        reid_list.push_back( QPair<QString,QString>( from_id, to_id ) );
    }
    log.stream( info ) << "All " << count << " lines passed, all tests passed. Now for the real thing";
    // Process each line of the file, stop on first error
    for ( int i = 0; i < reid_list.size(); ++i ) {
        try {
            Item::db_reid( reid_list[i].first, reid_list[i].second );
        }
        catch( ... ) {
            // Uhoh, even though we tried, we failed on something.
            QMessageBox::critical( this, tr( "Failure during changing of id" ),
                                  tr( "Some kind of error happened, while trying to change the id of item '%1' to id '%2'. This, and all changes following this in the file, have not been written to the database. When you close this dialog, a message describing the error will appear." ).arg( reid_list[i].first ).arg( reid_list[i].second ) );
            log.stream( error ) << "Some kind of error happened, while trying to change item with id '"
                                << reid_list[i].first << "' to id '" << reid_list[i].second << "'. If you see this error in the log, the code needs to be changed to be able to write the remaining changes to a file.";
            throw;
        }
    }
    QMessageBox::information( this, tr( "All changes committed" ),
                             tr( "All %1 changes of ids succesfully committed to the database." )
                             .arg( reid_list.size() ) );
}

void ReIdDialog::on_input_lookup_pushButton_clicked()
{
    Logger log( "void ReidDialog::on_input_lookup_pushButton_clicked()" );
    fromIdSet();
}

void ReIdDialog::on_input_fromId_lineEdit_textChanged( QString value ) {
    Logger log("void ReIdDialog::on_input_fromId_lineEdit_textChanged( QString )");
    ui->input_lookup_pushButton->setEnabled( !value.isEmpty() );
}


void ReIdDialog::on_input_toId_lineEdit_textChanged(QString )
{
    Logger log("void ReIdDialog::on_input_toId_lineEdit_textChanged(QString )");
    ui->buttonBox->button( QDialogButtonBox::Ok )->setEnabled( ui->input_fromId_lineEdit->isReadOnly() && !ui->input_toId_lineEdit->text().isEmpty() );
    ui->buttonBox->button( QDialogButtonBox::Apply )->setEnabled( ui->input_fromId_lineEdit->isReadOnly() && !ui->input_toId_lineEdit->text().isEmpty() );
}

// OK, as Apply, but also closes window
void ReIdDialog::on_buttonBox_accepted()
{
    Logger log( "void ReIdDialog::on_buttonBox_accepted()" );
    if ( doReId() )  {
        close();
    }
}

void ReIdDialog::on_buttonBox_clicked(QAbstractButton* button)
{
    Logger log( "void ReIdDialog::on_buttonBox_clicked(QAbstractButton* button)" );
    if ( button == ui->buttonBox->button( QDialogButtonBox::Apply) ) {
        log.stream() << "Apply pressed";
        doReId();
    } else {
        log.stream(  warn ) << "Unknown button pressed";
    }
}

void ReIdDialog::on_input_loadFile_pushButton_clicked()
{
    Logger log("void ReIdDialog::on_input_loadFile_pushButton_clicked()");
    readChangesFromFile();
}
