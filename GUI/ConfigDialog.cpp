/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtWidgets>
#include <Qt>

#include "ConfigDialog.h"
#include "ConfigPages.h"
#include "MainWindow.hpp"

ZonalPolicyParameters dummy_params;

ConfigDialog::ConfigDialog(MainWindow * _main_window, ZonalPolicyParameters& _params) :
        params(new ZonalPolicyParameters(_params)),
        main_window(_main_window),
        optsn_cntrl(new OptimiserController(main_window)),
        opt_needs_initialisation(true)
{
    optsn_cntrl->setParent(this);

    contentsWidget = new QListWidget;
    contentsWidget->setViewMode(QListView::IconMode);
    contentsWidget->setIconSize(QSize(96, 84));
    contentsWidget->setMovement(QListView::Static);
    contentsWidget->setMaximumWidth(128);
    contentsWidget->setSpacing(12);

    pagesWidget = new QStackedWidget;

    help_widget = new QTextEdit(this);

    ea_Page = new EAPage(this, help_widget);
    geon_setting_page = new GeonSettingsPage(this, help_widget);
    problem_spec_page = new ProblemSpecPage(this, help_widget);

    pagesWidget->addWidget(problem_spec_page);
    pagesWidget->addWidget(geon_setting_page);
    pagesWidget->addWidget(ea_Page);




    QPushButton *closeButton = new QPushButton(tr("Close"));
    QPushButton *saveAs_Button = new QPushButton(tr("Save As"));
    QPushButton *saveButton = new QPushButton(tr("Save"));
    QPushButton *test_Button = new QPushButton(tr("Test"));
    QPushButton *step_Button = new QPushButton(tr("Step"));
    QPushButton *runButton = new QPushButton(tr("Run"));

    createIcons();
    contentsWidget->setCurrentRow(0);

    connect(closeButton, &QAbstractButton::clicked, this, &QWidget::close);
    connect(saveButton, &QAbstractButton::clicked, this, &ConfigDialog::save);
    connect(saveAs_Button, &QAbstractButton::clicked, this, &ConfigDialog::saveAs);
    connect(runButton, &QAbstractButton::clicked, this, &ConfigDialog::run);
    connect(test_Button, &QAbstractButton::clicked, this, &ConfigDialog::test);
    connect(step_Button, &QAbstractButton::clicked, this, &ConfigDialog::step);


    QHBoxLayout *horizontalLayout = new QHBoxLayout;
    horizontalLayout->addWidget(contentsWidget);
    horizontalLayout->addWidget(pagesWidget, 1);
    horizontalLayout->addWidget(help_widget);

    QHBoxLayout *buttonsLayout = new QHBoxLayout;
    buttonsLayout->addStretch(1);
    buttonsLayout->addWidget(closeButton);
    buttonsLayout->addWidget(saveAs_Button);
    buttonsLayout->addWidget(saveButton);
    buttonsLayout->addWidget(test_Button);
    buttonsLayout->addWidget(step_Button);
    buttonsLayout->addWidget(runButton);


    QVBoxLayout *parametersLayout = new QVBoxLayout;
    parametersLayout->addLayout(horizontalLayout);
    parametersLayout->addStretch(1);
    parametersLayout->addSpacing(12);
    parametersLayout->addLayout(buttonsLayout);


//    QHBoxLayout * main_layout = new QHBoxLayout;
//    main_layout->addLayout(parametersLayout);
//    main_layout->addWidget(help_widget);


    setLayout(parametersLayout);

    setWindowTitle(tr("Config Dialog"));

    setAttribute(Qt::WA_DeleteOnClose);
    isUntitled = true;
}

void ConfigDialog::createIcons()
{
    QListWidgetItem *configButton = new QListWidgetItem(contentsWidget);
    configButton->setIcon(QIcon(":/images/config.png"));
    configButton->setText(tr("Problem\nSpecification"));
    configButton->setTextAlignment(Qt::AlignHCenter);
    configButton->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    QListWidgetItem *updateButton = new QListWidgetItem(contentsWidget);
    updateButton->setIcon(QIcon(":/images/update.png"));
    updateButton->setText(tr("Geonamica\nSettings"));
    updateButton->setTextAlignment(Qt::AlignHCenter);
    updateButton->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    QListWidgetItem *queryButton = new QListWidgetItem(contentsWidget);
    queryButton->setIcon(QIcon(":/images/query.png"));
    queryButton->setText(tr("Evolutionary\nAlgorithm\nParameters"));
    queryButton->setTextAlignment(Qt::AlignHCenter);
    queryButton->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    connect(contentsWidget, &QListWidget::currentItemChanged, this, &ConfigDialog::changePage);
}

void ConfigDialog::changePage(QListWidgetItem *current, QListWidgetItem *previous)
{
    if (!current)
        current = previous;

    pagesWidget->setCurrentIndex(contentsWidget->row(current));
}

void ConfigDialog::closeEvent(QCloseEvent *event)
{
    if (maybeSave()) {
        event->accept();
    } else {
        event->ignore();
    }
}

void ConfigDialog::initialise(int argc, char **argv)
{

    int err = parameter_loader.processOptions(argc, argv, *params);
    if (err != 0)
    {
        throw std::runtime_error("Could not parse options. Check file and path");
    }
    params->evaluator_id = 0;

}

void ConfigDialog::run()
{
//    std::cout << "Button clicked" << std::endl;
    if (this->opt_needs_initialisation)
    {
        optsn_cntrl->initialise(*params);
        opt_needs_initialisation = false;
    }
    optsn_cntrl->run();
}

void ConfigDialog::step()
{
    if (this->opt_needs_initialisation)
    {
        optsn_cntrl->initialise(*params);
        opt_needs_initialisation = false;
    }
    optsn_cntrl->step();
}

void ConfigDialog::test()
{
    if (this->opt_needs_initialisation)
    {
        optsn_cntrl->initialise(*params);
        opt_needs_initialisation = false;
    }
    optsn_cntrl->test();
}

void ConfigDialog::newFile()
{
    static int sequenceNumber = 1;

    isUntitled = true;
    curFile = tr("geonamica_optimisation_config%1.cfg").arg(sequenceNumber++);
    setWindowTitle("Configuration Diialogue " + curFile + "[*]");
    is_modified = false;
    opt_needs_initialisation = true;

    connect(this, &ConfigDialog::contentsChanged,
            this, &ConfigDialog::documentWasModified);
}

bool ConfigDialog::loadFile(const QString &fileName)
{

    //While reading in file, if long, change cursor.
    QApplication::setOverrideCursor(Qt::WaitCursor);
    // Open file and read in values into params struct.
    parameter_loader.processOptions(fileName.toStdString(), *params);

    problem_spec_page->updateObjectiveMaps(params->rel_path_obj_maps);
//    problem_spec_page->updateDiscountRate(params->discount_rate);
//    problem_spec_page->updateYearsCalculated(params->years_metric_calc);
//    problem_spec_page->updateStartYear(params->year_start_metrics);
//    problem_spec_page->updateEndYear(params->year_end_metrics);
    problem_spec_page->updateZoneDelineationMap(QString::fromStdString(params->rel_path_zones_delineation_map));
    problem_spec_page->updateZonalLayerMap(QString::fromStdString(params->rel_path_zonal_map));
    problem_spec_page->updateXpathDVs(params->xpath_dvs);
    problem_spec_page->updateObjModules(params->objectives_plugins);

    geon_setting_page->updateTimeoutCmd(QString::fromStdString(params->timout_cmd));
    geon_setting_page->updateWineCmd(QString::fromStdString(params->wine_cmd));
    geon_setting_page->updateGeonCmd(QString::fromStdString(params->geonamica_cmd));
    geon_setting_page->updateworkingDir(QString::fromStdString(params->working_dir.first));
    geon_setting_page->updateWinePrefix(QString::fromStdString(params->wine_prefix_path.first));
    geon_setting_page->updateSaveDir(QString::fromStdString(params->save_dir.first));
    geon_setting_page->updateWhetherPrefixEnvVarSet(params->set_prefix_path);
    geon_setting_page->updateWindowsEnvVar(QString::fromStdString(params->windows_env_var));
    geon_setting_page->updateWhetherLog(params->is_logging);
    geon_setting_page->updateGeoprojDir(QString::fromStdString(params->template_project_dir.first));
    geon_setting_page->updateGeoprojFile(QString::fromStdString(params->rel_path_geoproj));
    geon_setting_page->updateObjLogFile(QString::fromStdString(params->rel_path_log_specification_obj));
    geon_setting_page->updatePostOptPrintLogFile(QString::fromStdString(params->rel_path_log_specification_save));
//    geon_setting_page->updateStartYear(params->year_start_saving);
//    geon_setting_page->updateEndYear(params->year_end_saving);
    geon_setting_page->updateNumberReplicates(params->replicates);
    geon_setting_page->updateOuputLogMaps(params->save_maps);

    ea_Page->updatePop(params->pop_size);
    ea_Page->updateHyprvolTerm(params->max_gen_hvol);
    ea_Page->updateMaxGenTerm(params->max_gen);
    ea_Page->updateReseedFile(QString::fromStdString(params->restart_pop_file.first));
    ea_Page->updateLoggingFreq(params->save_freq);



    //Return cursor to normal.
    QApplication::restoreOverrideCursor();

    setCurrentFile(fileName);

    is_modified = false;
    opt_needs_initialisation = true;

    //
    connect(this, &ConfigDialog::contentsChanged,
            this, &ConfigDialog::documentWasModified);

    return true;
}

bool ConfigDialog::save()
{
    if (isUntitled) {
        return saveAs();
    } else {
        return saveFile(curFile);
    }
}

bool ConfigDialog::saveAs()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"),
                                                    curFile);
    if (fileName.isEmpty())
        return false;

    return saveFile(fileName);
}

bool ConfigDialog::saveFile(const QString &fileName)
{

    QApplication::setOverrideCursor(Qt::WaitCursor);

    //Save params to file here.
    parameter_loader.saveOptions(fileName.toStdString(), *(this->params));

    QApplication::restoreOverrideCursor();

    setCurrentFile(fileName);
    is_modified = false;
    return true;
}

QString ConfigDialog::userFriendlyCurrentFile()
{
    return strippedName(curFile);
}


void ConfigDialog::documentWasModified()
{
    setWindowModified(true);
}

bool ConfigDialog::maybeSave()
{
    if (!this->is_modified)
        return true;
    const QMessageBox::StandardButton ret
            = QMessageBox::warning(this, tr("MDI"),
                                   tr("'%1' has been modified.\n"
                                              "Do you want to save your changes?")
                                           .arg(userFriendlyCurrentFile()),
                                   QMessageBox::Save | QMessageBox::Discard
                                   | QMessageBox::Cancel);
    switch (ret) {
        case QMessageBox::Save:
            return save();
        case QMessageBox::Cancel:
            return false;
        default:
            break;
    }
    return true;
}

void ConfigDialog::setCurrentFile(const QString &fileName)
{
    curFile = QFileInfo(fileName).canonicalFilePath();
    isUntitled = false;
    this->is_modified = false;
    setWindowModified(false);
    setWindowTitle(userFriendlyCurrentFile() + "[*]");
}

QString ConfigDialog::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

void ConfigDialog::changeTimeoutCmd(QString new_val)
{
    this->params->timout_cmd = new_val.toStdString();
    is_modified = true;
    opt_needs_initialisation = true;
}

void ConfigDialog::changeWineCmd(QString new_val)
{
    this->params->wine_cmd = new_val.toStdString();
    is_modified = true;
    opt_needs_initialisation = true;
}

void ConfigDialog::changeGeonCmd(QString new_val)
{
    this->params->geonamica_cmd = new_val.toStdString();
    is_modified = true;
    opt_needs_initialisation = true;
}

void ConfigDialog::changeResetAndSave(int state)
{
    if (state == Qt::Unchecked) this->params->with_reset_and_save = false;
    else this->params->with_reset_and_save = true;
    is_modified = true;
    opt_needs_initialisation = true;
}

void ConfigDialog::changeGeoprojDirectory(QString new_val)
{
    this->params->template_project_dir.first = new_val.toStdString();
    is_modified = true;
    opt_needs_initialisation = true;

}

void ConfigDialog::changeWorkingDirectory(QString new_val)
{
    this->params->working_dir.first = new_val.toStdString();
    is_modified = true;
    opt_needs_initialisation = true;
}

void ConfigDialog::changePrefixPath(QString new_val)
{
    this->params->wine_prefix_path.first = new_val.toStdString();
    is_modified = true;
    opt_needs_initialisation = true;
}

void ConfigDialog::changeWhetherPrefixEnvVarSet(int state)
{
    if (state == Qt::Unchecked) this->params->set_prefix_path = false;
    else this->params->set_prefix_path = true;
    is_modified = true;
    opt_needs_initialisation = true;
}

void ConfigDialog::changeWindowsEnvVar(QString new_val)
{
    this->params->windows_env_var = new_val.toStdString();
    is_modified = true;
    opt_needs_initialisation = true;
}

void ConfigDialog::changeGeoprojectFile(QString new_val)
{
    this->params->rel_path_geoproj = new_val.toStdString();
    is_modified = true;
    opt_needs_initialisation = true;
}

void ConfigDialog::changeLogFileForObjective(QString new_val)
{
    this->params->rel_path_log_specification_obj = new_val.toStdString();
    is_modified = true;
    opt_needs_initialisation = true;
}

void ConfigDialog::changeLogFileForPostOptimisationSave(QString new_val)
{
    this->params->rel_path_log_specification_save = new_val.toStdString();
    is_modified = true;
    opt_needs_initialisation = true;
}

void ConfigDialog::changeNumberReplicates(int new_val)
{
    this->params->replicates = new_val;
    is_modified = true;
    opt_needs_initialisation = true;
}

void ConfigDialog::changeObjectiveMaps(QVector<QString> new_vals)
{
    this->params->rel_path_obj_maps.clear();
    QVectorIterator<QString> i(new_vals);
    while (i.hasNext())
        this->params->rel_path_obj_maps.push_back(i.next().toStdString());
    is_modified = true;
    opt_needs_initialisation = true;
}

void ConfigDialog::changeMetricYears(QVector<QString> new_vals)
{
    this->params->years_metric_calc.clear();
    QVectorIterator<QString> i(new_vals);
    while (i.hasNext())
        this->params->years_metric_calc.push_back(i.next().toInt());
    is_modified = true;
    opt_needs_initialisation = true;
}

//void ConfigDialog::changeObjectiveMaps(QListWidgetItem *current, QListWidgetItem *previous)
//{
//    if (previous != nullptr)
//    {
//        auto previous_it = std::find(this->params->rel_path_obj_maps.begin(), this->params->rel_path_obj_maps.end(),
//                                     previous->text().toStdString());
//        if (previous_it != this->params->rel_path_obj_maps.end())
//        {
//            this->params->rel_path_obj_maps.erase(previous_it);
//        }
//    }
//    this->params->rel_path_obj_maps.push_back(current->text().toStdString());
//}

void ConfigDialog::changeZonalLayer(QString new_val)
{
    this->params->rel_path_zonal_map = new_val.toStdString();
    is_modified = true;
    opt_needs_initialisation = true;
}

void ConfigDialog::changeZoneDelineation(QString new_val)
{
    this->params->rel_path_zones_delineation_map = new_val.toStdString();
    is_modified = true;
    opt_needs_initialisation = true;
}

void ConfigDialog::changeXPathDVs(QVector<QString> new_vals)
{
    this->params->xpath_dvs.clear();
    QVectorIterator<QString> i(new_vals);
    while (i.hasNext())
        this->params->xpath_dvs.push_back(i.next().toStdString());
    is_modified = true;
    opt_needs_initialisation = true;
}

//void ConfigDialog::changeXPathDVs(QListWidgetItem *current, QListWidgetItem *previous)
//{
//    if (previous != nullptr)
//    {
//        auto previous_it = std::find(this->params->xpath_dvs.begin(), this->params->xpath_dvs.end(),
//                                     previous->text().toStdString());
//        if (previous_it != this->params->xpath_dvs.end())
//        {
//            this->params->xpath_dvs.erase(previous_it);
//        }
//    }
//    this->params->xpath_dvs.push_back(current->text().toStdString());
//}

void ConfigDialog::changeDoLog(int state)
{
    if (state == Qt::Unchecked) this->params->is_logging = false;
    else this->params->is_logging = true;
    is_modified = true;
    opt_needs_initialisation = true;
}

void ConfigDialog::changeSaveDir(QString new_val)
{
    this->params->save_dir.first = new_val.toStdString();
    is_modified = true;
    opt_needs_initialisation = true;
}

void ConfigDialog::changeSaveMaps(QVector<QString> new_vals)
{
    this->params->save_maps.clear();
    QVectorIterator<QString> i(new_vals);
    while (i.hasNext())
        this->params->save_maps.push_back(i.next().toStdString());
    is_modified = true;
    opt_needs_initialisation = true;
}

//void ConfigDialog::changeSaveMaps(QListWidgetItem *current, QListWidgetItem *previous)
//{
//    if (previous != nullptr)
//    {
//        auto previous_it = std::find(this->params->save_maps.begin(), this->params->save_maps.end(),
//                                     previous->text().toStdString());
//        if (previous_it != this->params->save_maps.end())
//        {
//            this->params->save_maps.erase(previous_it);
//        }
//    }
//    this->params->save_maps.push_back(current->text().toStdString());
//}

void ConfigDialog::changePopSize(int new_val)
{
    this->params->pop_size = new_val;
    is_modified = true;
    opt_needs_initialisation = true;
}

void ConfigDialog::changeMaxGenHypImprove(int new_val)
{
    this->params->max_gen_hvol = new_val;
    is_modified = true;
    opt_needs_initialisation = true;
}

void ConfigDialog::changeMaxGen(int new_val)
{
    this->params->max_gen = new_val;
    is_modified = true;
    opt_needs_initialisation = true;
}

void ConfigDialog::changeSaveFreq(int new_val)
{
    this->params->save_freq = new_val;
    is_modified = true;
    opt_needs_initialisation = true;
}

void ConfigDialog::changeRessed(QString new_val)
{
    this->params->restart_pop_file.first = new_val.toStdString();
    is_modified = true;
    opt_needs_initialisation = true;
}

//void ConfigDialog::changeYearStartSave(QString new_val)
//{
//    bool ok;
//    double new_val_d = new_val.toDouble(&ok);
//    if (ok && new_val_d < 100000)
//    {
//        this->params->year_start_saving = new_val_d;
//    }
//    else
//    {
//        QMessageBox msgBox;
//        msgBox.setText("Year start needs to be a an integer <XXXX> year");
//        msgBox.exec();
//    }
//    is_modified = true;
//    opt_needs_initialisation = true;
//}

//void ConfigDialog::changeYearEndSave(QString new_val)
//{
//    bool ok;
//    double new_val_d = new_val.toDouble(&ok);
//    if (ok && new_val_d < 100000)
//    {
//        this->params->year_end_saving = new_val_d;
//    }
//    else
//    {
//        QMessageBox msgBox;
//        msgBox.setText("Year start needs to be a an integer <XXXX> year");
//        msgBox.exec();
//    }
//    is_modified = true;
//    opt_needs_initialisation = true;
//}

//void ConfigDialog::changeYearStartMetrics(QString new_val)
//{
//    bool ok;
//    double new_val_d = new_val.toDouble(&ok);
//    if (ok && new_val_d < 100000)
//    {
//        this->params->year_start_metrics = new_val_d;
//    }
//    else
//    {
//        QMessageBox msgBox;
//        msgBox.setText("Year start needs to be a an integer <XXXX> year");
//        msgBox.exec();
//    }
//    is_modified = true;
//    opt_needs_initialisation = true;
//}

//void ConfigDialog::changeYearEndMetrics(QString new_val)
//{
//    bool ok;
//    double new_val_d = new_val.toDouble(&ok);
//    if (ok && new_val_d < 100000)
//    {
//        this->params->year_end_metrics = new_val_d;
//    }
//    else
//    {
//        QMessageBox msgBox;
//        msgBox.setText("Year start needs to be a an integer <XXXX> year");
//        msgBox.exec();
//    }
//    is_modified = true;
//    opt_needs_initialisation = true;
//}

//void ConfigDialog::changeDiscountRate(QString new_val)
//{
//    bool ok;
//    double new_val_d = new_val.toDouble(&ok);
//    if (ok && new_val_d >= 0 && new_val_d <= 1)
//    {
//        this->params->discount_rate = new_val_d;
//    }
//    else
//    {
//        QMessageBox msgBox;
//        msgBox.setText("Discount rate needs to be an floating point number between 0 and 1");
//        msgBox.exec();
//    }
//    is_modified = true;
//    opt_needs_initialisation = true;
//}

//void ConfigDialog::changeDiscountYearZero(QString new_val)
//{
//    bool ok;
//    int new_val_i = new_val.toInt(&ok);
//    if (ok && new_val_i < 100000)
//    {
//        this->params->discount_start_year = new_val_d;
//    }
//    else
//    {
//        QMessageBox msgBox;
//        msgBox.setText("The year in which present value is counted for needs to be a an integer <XXXX> year");
//        msgBox.exec();
//    }
//    is_modified = true;
//    opt_needs_initialisation = true;
//}



void ConfigDialog::changeObjModules(QVector<QString> new_vals)
{
    this->params->objectives_plugins.clear();
    QVectorIterator<QString> i(new_vals);
    while (i.hasNext())
        this->params->objectives_plugins.push_back(i.next().toStdString());
    is_modified = true;
    opt_needs_initialisation = true;
}

