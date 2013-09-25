/*
    This file is part of JapKatsuyou project; an application that provide
    Japanese verb conjugation

    Copyright (C) 2013  Abdelkrime Aries <kariminfo0@gmail.com>
    Copyright (C) 2013  DzCoding group (JapKatsuyou team)

    Authors:
            Abdelkrime Aries <kariminfo0@gmail.com>
            Zakaria Smahi <zakaria08esi@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "jpconjmain.h"
#include "ui_jpconjmain.h"

/*!
 * \class jpconjmain
 * This class is used to manage the main window of the application.
 */



/*******************************************************
 *                    PUBLIC
 *******************************************************/

/*!
 * \brief jpconjmain::jpconjmain The constructor of the main window
 * \param parent
 */
jpconjmain::jpconjmain(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::jpconjmain)
{
    ui->setupUi(this);
    doInit();
}



/*!
 * \brief jpconjmain::~jpconjmain The destructor of the main window
 */
jpconjmain::~jpconjmain()
{
    delete ui;
}



/*******************************************************
 *                    PRIVATE
 *******************************************************/

/*!
 * \brief jpconjmain::doInit Initialize the components of the main window
 *
 * This function sets the language and direction of the main window.
 * You can call it in the constructor of this window.
 */
void jpconjmain::doInit()
{
    qDebug()<< QString(VERSION);
    libjpconjlink::Init();
    Language::loadTranslations();
    Language::mainWindowDirection(this);
    ui->menu_View->addAction(ui->mainTool->toggleViewAction());
    ui->menu_View->addAction(ui->search->toggleViewAction());
    //ui->showt->setLayoutDirection(Qt::LeftToRight);

    QPalette palette;
    palette.setBrush(QPalette::Base, Qt::transparent);
    ui->basicConj->page()->setPalette(palette);
    ui->basicConj->setAttribute(Qt::WA_OpaquePaintEvent, false);
    ui->standardConj->page()->setPalette(palette);
    ui->standardConj->setAttribute(Qt::WA_OpaquePaintEvent, false);

    setCSS(ui->basicConj, "DzStyle.css");
    setCSS(ui->standardConj, "DzStyle.css");
    setCSS(ui->complexConj, "DzStyle.css");
}



/*!
 * \brief jpconjmain::openAbout This function is used to call "about" dialog box.
 */
void jpconjmain::openAbout()
{
    winAbout = new About(this);
    winAbout->setLayoutDirection(this->layoutDirection());
    winAbout->setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);
    winAbout->show();

}



/*!
 * \brief jpconjmain::openPref This function is used to call "preferences" dialog box.
 */
void jpconjmain::openPref()
{
    winPref = new Preference(this);
    winPref->setLayoutDirection(this->layoutDirection());
    winPref->setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);
    winPref->show();
}



/*!
 * \brief jpconjmain::doConj This function is used to show different conjugation forms
 *
 * This function search for the existance of the verb, using Edict2 module.
 * Then, it calls for:
 * - jpconjmain::basicConjugation To show the standard and basic conjugation forms.
 * - jpconjmain::complexConjugation To show the complex conjugation forms.
 */
void jpconjmain::doConj()
{

    QString verb = ui->inputt->text();

    Edict2 edict2;

    EdictType type = edict2.find(verb);

    ui->msgt->setText(Msg::getVerbTypeDesc(type));

    if (type > 0) {
        complexConjugation(verb, type);
        basicConjugation(verb, type);
    }

}



/*!
 * \brief jpconjmain::basicConjugation Used to show the standard and basic conjugation forms.
 * \param verb The verb in dictionary form (u-form), eg. 食べる, 飲む, 行く, 来る, etc.
 * \param type The Edict2 type of the verb (See: VerbType::EdictType)
 */
void jpconjmain::basicConjugation(QString verb, EdictType type)
{
    QString basicConjHTML = readHtmlFile(":/output/basicConj");
    QString standardConjHTML = readHtmlFile(":/output/standardConj");

    {//begin: calculated strings
        //! [Doxygen: basicFormsMap example]
        QMap<KForm, QString> basicForms = Msg::basicFormsMap();
        foreach (KForm form, basicForms.keys()){
            QStringList conj = libjpconjlink::katsuyou(verb, type, form).split("|");
            QString str = basicForms.value(form) + "&";

            standardConjHTML.replace("&stem_" + str, conj[0]);
            standardConjHTML.replace("&suffix_" + str, conj[1]);

            basicConjHTML.replace("&basic_" + str, conj[0] + conj[1]);
            basicConjHTML.replace("&_Name_" + str, Msg::getBasicFormName(form));
        }
        //! [Doxygen: basicFormsMap example]
    }//end: calculated strings

    {//begin: constant strings
        QList<QString> verbStrings =  Msg::verbStringsList();
        for (int i=0; i< verbStrings.size(); i++){
            QString translation = Msg::getTranslatedString(i);
            standardConjHTML.replace(verbStrings[i], translation);
            basicConjHTML.replace(verbStrings[i], translation);
        }
    }//end: constant strings

    ui->standardConj->setHtml(standardConjHTML);
    ui->basicConj->setHtml(basicConjHTML);
}



/*!
 * \brief jpconjmain::complexConjugation Used to show the complex conjugation forms.
 * \param verb The verb in dictionary form (u-form), eg. 食べる, 飲む, 行く, 来る, etc.
 * \param type The Edict2 type of the verb (See: VerbType::EdictType)
 */
void jpconjmain::complexConjugation(QString verb, EdictType type)
{
    QString complexConjHTML = readHtmlFile(":/output/complexConj");

    {//begin: constant strings
        complexConjHTML.replace("&_Polite&", Msg::getVerbPolitenessName(VConjugate::_Polite));
        complexConjHTML.replace("&_Plain&", Msg::getVerbPolitenessName(VConjugate::_Plain));
        complexConjHTML.replace("&_Affirmative&", Msg::getVerbPolarityName(VConjugate::_Affirmative));
        complexConjHTML.replace("&_Negative&", Msg::getVerbPolarityName(VConjugate::_Negative));

        complexConjHTML.replace("&_Form&", Msg::getTranslatedString(4));

        QMap<CForm, QString> complexForms = Msg::complexFormsMap();
        foreach (CForm form, complexForms.keys()){
            QString str = complexForms.value(form) + "&";

            complexConjHTML.replace("&_Form_" + str, Msg::getVerbFormName(form));
            complexConjHTML.replace("&_Tip_" + str, Msg::getVerbFormDesc(form));
            complexConjHTML.replace("&PoA_" + str,
                                     libjpconjlink::conjugate(verb, type, form, VConjugate::_Polite, VConjugate::_Affirmative).remove("|"));
            complexConjHTML.replace("&PoN_" + str,
                                     libjpconjlink::conjugate(verb, type, form, VConjugate::_Polite, VConjugate::_Negative).remove("|"));
            complexConjHTML.replace("&PlA_" + str,
                                     libjpconjlink::conjugate(verb, type, form, VConjugate::_Plain, VConjugate::_Affirmative).remove("|"));
            complexConjHTML.replace("&PlN_" + str,
                                     libjpconjlink::conjugate(verb, type, form, VConjugate::_Plain, VConjugate::_Negative).remove("|"));
        }

    }//end: constant strings

    ui->complexConj->setHtml(complexConjHTML);

}



/*!
 * \brief jpconjmain::readHtmlFile Reads an HTML file and return a QString
 * \param URL URL of the HTML file, we want to read.
 * \return A QString which is the content of this HTML file.
 */
QString jpconjmain::readHtmlFile(QString URL)
{
    QString result="<meta http-equiv=\"content-type\" content=\"text/html; charset=UTF-8\" />";
    QFile HtmlFile(URL);

    if (HtmlFile.open(QIODevice::ReadOnly | QIODevice::Text)){
        QTextStream htmlStream(&HtmlFile);
        result = htmlStream.readAll();
    }

    return result;
}



/*!
 * \brief jpconjmain::setCSS Set a user defined CSS to the QWebView
 * \param webView The QWebView we want to set the content CSS.
 * \param nameCSS The name of the CSS located in <dataFolder>/styles/ with the extension ".css"
 */
void jpconjmain::setCSS(QWebView * webView, QString nameCSS)
{
    QString cssfile = QDir(QString(dataFolder)).absolutePath() + "/styles/" + nameCSS;
    QWebSettings * settings = webView->settings();
    settings->setUserStyleSheetUrl(QUrl::fromLocalFile(cssfile));
}

/*******************************************************
 *                    PROTECTED
 *******************************************************/

/*!
 * \brief jpconjmain::changeEvent The event treated her is the change of language.
 * \param event
 */
void jpconjmain::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::LanguageChange)
    {
        //Msg::updateMsg();
        ui->retranslateUi(this);
        Language::mainWindowDirection(this);

    }
    QMainWindow::changeEvent(event);
}



/*******************************************************
 *                   PRIVATE SLOTS
 *******************************************************/

void jpconjmain::on_action_Close_triggered()
{
    this->close();
}

void jpconjmain::on_btn_conj_clicked()
{
    doConj();
}

void jpconjmain::on_action_About_triggered()
{
    openAbout();
}

void jpconjmain::on_action_Preference_triggered()
{
    openPref();
}

void jpconjmain::on_actionHelp_Content_triggered()
{

}
