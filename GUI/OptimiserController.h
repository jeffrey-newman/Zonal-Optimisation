//
// Created by a1091793 on 8/7/17.
//

#ifndef WDS_OPT_OPTIMISERCONTROLLER_H
#define WDS_OPT_OPTIMISERCONTROLLER_H

#include <QThread>
#include "TimeSeriesQtChart.h"
#include "ParetoFrontQtChart.h"
#include <QtCharts/QChartView>
#include "GeonamicaPolicyParametersQtMetaTyping.hpp"
#include "MainWindow.hpp"
#include "OptimiserWorker.h"


class OptimiserController : public QObject
{
    Q_OBJECT
    QThread optimiser_thread;

public:
    OptimiserController(MainWindow * main_window);

    ~OptimiserController();

    public slots:
    void onError(QString what);

    void initialise(GeonamicaPolicyParameters _params);
    void run();
    void step();
    void test(GeonamicaPolicyParameters _params);
    void evalReseedPop();

signals:
    void runOptimisation();
    void testOptimisation(GeonamicaPolicyParameters _params);
    void intialiseOptimisation(GeonamicaPolicyParameters _params);
    void stepOptimisation();
    void evalReseedPopInWorker();
//    void terminateOptimisation();


private:
    TimeSeriesQtChart * hypervolume_chart;
    ParetoFrontQtChart * front_chart;
    QtCharts::QChartView * hypervolume_chart_view;
    QtCharts::QChartView * paretofront_chart_view;
    OptimiserWorker * worker;

};


#endif //WDS_OPT_OPTIMISERCONTROLLER_H
