/*
 *   File name: MimeCategorizer.cpp
 *   Summary:	Support classes for QZedStat
 *   License:	GPL V2 - See file LICENSE for details.
 *
 *   Author:	Stefan Hundhammer <Stefan.Hundhammer@gmx.de>
 */


#include "MimeCategorizer.h"
#include "FileInfo.h"
#include "Settings.h"
#include "SettingsHelpers.h"
#include "Logger.h"
#include "Exception.h"


using namespace QZedStat;

MimeCategorizer::MimeCategorizer( QObject * parent ):
    QObject( parent ),
    _mapsDirty( true )
{
    readSettings();
}


MimeCategorizer::~MimeCategorizer()
{
    writeSettings();
    clear();
}


void MimeCategorizer::clear()
{
    qDeleteAll( _categories );
    _categories.clear();
    _mapsDirty = true;
}


MimeCategory * MimeCategorizer::category( FileInfo * item )
{
    CHECK_PTR  ( item );
    CHECK_MAGIC( item );

    if ( item->isDir() || item->isDirInfo() )
	return 0;
    else
	return category( item->name() );
}


MimeCategory * MimeCategorizer::category( const QString & filename )
{
    if ( filename.isEmpty() )
	return 0;

    // Build suffix maps for fast lookup

    if ( _mapsDirty )
	buildMaps();

    MimeCategory * category = 0;

    // Find the filename suffix: Everything after the first '.'
    QString suffix = filename.section( '.', 1 );

    while ( ! suffix.isEmpty() )
    {
	// Try case sensitive first

	category = _caseSensitiveSuffixMap.value( suffix, 0 );

	if ( ! category )
	    category = _caseInsensitiveSuffixMap.value( suffix.toLower(), 0 );

	// No match so far? Try the next suffix. Some files might have more
	// than one, e.g., "tar.bz2" - if there is no match for "tar.bz2",
	// there might be one for just "bz2".

	suffix = suffix.section( '.', 1 );
    }

    if ( ! category ) // No match yet?
	category = matchPatterns( filename );

#if 0
    if ( category )
	logVerbose() << "Found " << category << " for " << filename << endl;
#endif

    return category;
}


MimeCategory * MimeCategorizer::matchPatterns( const QString & filename ) const
{
    foreach ( MimeCategory * category, _categories )
    {
	if ( category )
	{
	    foreach ( const QRegExp & pattern, category->patternList() )
	    {
		if ( pattern.exactMatch( filename ) )
		    return category;
	    }
	}
    }

    return 0; // No match
}


void MimeCategorizer::add( MimeCategory * category )
{
    CHECK_PTR( category );

    _categories << category;
    _mapsDirty = true;
}


void MimeCategorizer::remove( MimeCategory * category )
{
    CHECK_PTR( category );

    _categories.removeAll( category );
    delete category;
    _mapsDirty = true;
}


void MimeCategorizer::buildMaps()
{
    _caseInsensitiveSuffixMap.clear();
    _caseSensitiveSuffixMap.clear();

    foreach ( MimeCategory * category, _categories )
    {
	CHECK_PTR( category );

	addSuffixes( _caseInsensitiveSuffixMap, category, category->caseInsensitiveSuffixList() );
	addSuffixes( _caseSensitiveSuffixMap,	category, category->caseSensitiveSuffixList()	);
    }

    _mapsDirty = false;
}


void MimeCategorizer::addSuffixes( QMap<QString, MimeCategory *> & suffixMap,
				   MimeCategory			 * category,
				   const QStringList		 & suffixList  )
{
    foreach ( const QString & suffix, suffixList )
    {
	if ( suffixMap.contains( suffix ) )
	{
	    logError() << "Duplicate suffix: " << suffix << " for "
		       << suffixMap.value( suffix ) << " and " << category
		       << endl;
	}
	else
	{
	    suffixMap[ suffix ] = category;
	}
    }
}


void MimeCategorizer::readSettings()
{
    MimeCategorySettings settings;
    QStringList mimeCategoryGroups = settings.findGroups( settings.groupPrefix() );

    clear();

    // Read all settings groups [MimeCategory_xx] that were found

    foreach ( const QString & groupName, mimeCategoryGroups )
    {
	settings.beginGroup( groupName );

	QString name  = settings.value( "Name", groupName ).toString();
	QColor	color = readColorEntry( settings, "Color", QColor( "#b0b0b0" ) );
	QStringList patternsCaseInsensitive = settings.value( "PatternsCaseInsensitive" ).toStringList();
	QStringList patternsCaseSensitive   = settings.value( "PatternsCaseSensitive"	).toStringList();

	MimeCategory * category = new MimeCategory( name, color );
	CHECK_NEW( category );

	add( category );
	category->addPatterns( patternsCaseInsensitive, Qt::CaseInsensitive );
	category->addPatterns( patternsCaseSensitive,	Qt::CaseSensitive   );

	settings.endGroup(); // [MimeCategory_01], [MimeCategory_02], ...
    }

    if ( _categories.isEmpty() )
	addDefaultCategories();
}


void MimeCategorizer::writeSettings()
{
    MimeCategorySettings settings;

    // Remove all leftover cleanup descriptions
    settings.removeGroups( settings.groupPrefix() );

    for ( int i=0; i < _categories.size(); ++i )
    {
	QString groupName;
	groupName.sprintf( "MimeCategory_%02d", i+1 );
	settings.beginGroup( groupName );

	MimeCategory * category = _categories.at(i);

	settings.setValue( "Name", category->name() );
	writeColorEntry( settings, "Color", category->color() );

	QStringList patterns = category->humanReadablePatternList( Qt::CaseInsensitive );

	if ( patterns.isEmpty() )
	    patterns << "";

	settings.setValue( "PatternsCaseInsensitive", patterns );

	patterns = category->humanReadablePatternList( Qt::CaseSensitive );

	if ( patterns.isEmpty() )
	    patterns << "";

	settings.setValue( "PatternsCaseSensitive", patterns );

	settings.endGroup(); // [MimeCategory_01], [MimeCategory_02], ...
    }
}


void MimeCategorizer::addDefaultCategories()
{   
    MimeCategory * ada = new MimeCategory( tr( "Ada" ), QColor( 0x00, 0x54, 0xe1 ) );
    CHECK_NEW( ada );
    add( ada );

    ada->addSuffixes( QStringList()
               << "ada"
               << "a"
               << "adb"
               << "ads"
               );


    MimeCategory * asp = new MimeCategory( tr( "ASP, ASP.NET" ), QColor( 0x00, 0x40, 0x80 ) );
    CHECK_NEW( asp );
    add( asp );

    asp->addSuffixes( QStringList()
               << "asp"
               << "aspx"
			   );


    MimeCategory * bash = new MimeCategory( tr( "Bash" ), QColor( 0x00, 0xdd, 0xff ) );
    CHECK_NEW( bash );
    add( bash );

    bash->addSuffixes( QStringList()
               << "sh"
               << "ksh"
               );


    MimeCategory * csh = new MimeCategory( tr( "C Shell" ), QColor( 0xff, 0xfd, 0xce ) );
    CHECK_NEW( csh );
    add( csh );

    csh->addSuffixes( QStringList()
               << "csh"
               << "tcsh"
               );

    MimeCategory * cshrp = new MimeCategory( tr( "C#" ), QColor( 0x21, 0x1b, 0x0c ) );
    CHECK_NEW( cshrp );
    add( cshrp );

    cshrp->addSuffixes( QStringList()
               << "cs"
               );

    MimeCategory * cee = new MimeCategory( tr( "C/C++" ), QColor( 0x17, 0x00, 0x00 ) );
    CHECK_NEW(cee );
    add( cee );

    cee->addSuffixes( QStringList()
               << "cpp"
               << "c"
               << "h"
               << "hpp"
               << "cc"
               << "hh"
               );

    MimeCategory * cldf = new MimeCategory( tr( "ColdFusion" ), QColor( 0x00, 0xf1a, 0x64 ) );
    CHECK_NEW(cldf );
    add( cldf );

    cldf->addSuffixes( QStringList()
               << "cfs"
               );

    MimeCategory * css = new MimeCategory( tr( "CSS" ), QColor( 0x95, 0xbb, 0xef ) );
    CHECK_NEW(css );
    add( css );

    css->addSuffixes( QStringList()
               << "css"
               );

    MimeCategory * frtrn = new MimeCategory( tr( "Fortran" ), QColor( 0x00, 0x1d, 0x36 ) );
    CHECK_NEW(frtrn );
    add( frtrn );

    frtrn->addSuffixes( QStringList()
               << "f"
               << "for"
               << "f77"
               << "f90"
               << "f95"
               << "f03"
               << "hpf"
               );

    MimeCategory * html = new MimeCategory( tr( "HTML" ), QColor( 0xdc, 0xff, 0xff ) );
    CHECK_NEW(html );
    add( html );

    html->addSuffixes( QStringList()
               << "htm"
               << "html"
               << "shtml"
               << "stm"
               << "sht"
               << "oth"
               << "xhtml"
               );

    MimeCategory * java = new MimeCategory( tr( "Java" ), QColor( 0xaf, 0x8e, 0x00 ) );
    CHECK_NEW(java );
    add( java );

    java->addSuffixes( QStringList()
               << "Java"
               );

    MimeCategory * jvs = new MimeCategory( tr( "JavaScript" ), QColor( 0x3e, 0x3b, 0x34 ) );
    CHECK_NEW(jvs );
    add( jvs );

    jvs->addSuffixes( QStringList()
               << "js"
               );

    MimeCategory * pscl = new MimeCategory( tr( "Pascal" ), QColor( 0x24, 0x00, 0x00 ) );
    CHECK_NEW(pscl );
    add( pscl );

    pscl->addSuffixes( QStringList()
               << "pas"
               << "p"
               << "pp"
               << "pa3"
               << "pa4"
               << "pa5"
               );

    MimeCategory * prl = new MimeCategory( tr( "Perl" ), QColor( 0xbd, 0xc7, 0xd9 ) );
    CHECK_NEW(prl );
    add( prl );

    prl->addSuffixes( QStringList()
               << "pl"
               << "pm"
               );

    MimeCategory * php = new MimeCategory( tr( "PHP" ), QColor( 0x49, 0x60, 0x7f ) );
    CHECK_NEW(php );
    add( php );

    php->addSuffixes( QStringList()
               << "php"
               );

    MimeCategory * pythn = new MimeCategory( tr( "Python" ), QColor( 0x00, 0x8d, 0xff ) );
    CHECK_NEW(pythn );
    add( pythn );

    pythn->addSuffixes( QStringList()
               << "py"
               );

    MimeCategory * rby = new MimeCategory( tr( "Ruby" ), QColor( 0xff, 0xfb, 0x58 ) );
    CHECK_NEW(rby );
    add( rby );

    rby->addSuffixes( QStringList()
               << "rb"
               );

    MimeCategory * sql = new MimeCategory( tr( "SQL" ), QColor( 0x88, 0x75, 0x43 ) );
    CHECK_NEW(sql );
    add( sql );

    sql->addSuffixes( QStringList()
               << "sql"
               );

    MimeCategory * vb = new MimeCategory( tr( "VB" ), QColor( 0x41, 0x22, 0x00 ) );
    CHECK_NEW(vb );
    add( vb );

    vb->addSuffixes( QStringList()
               << "vb"
               << "frm"
               << "mod"
               << "cls"
               << "bas"
               );

    MimeCategory * vhdl = new MimeCategory( tr( "VHDL" ), QColor( 0x26, 0x00, 0x00 ) );
    CHECK_NEW(vhdl );
    add( vhdl );

    vhdl->addSuffixes( QStringList()
               << "vhd"
               << "vhdl"
               );

}
