#ifndef STATISTICS_H
#define STATISTICS_H

#include <QFile>
#include <QTextStream>
#include <QString>
#include <QMessageBox>
#include <QDir>
#include <QApplication>

#include <iostream>
#include <vector>
#include <sys/time.h>

class statistics{

public:

/*=== Public Statistic Functions ===*/

    statistics(const QString book, int numberOfPages, int linesPerPage);
    ~statistics();
    void startPage(int pagenum);
    void endPage(int pagenum);
    void disableStats();
    void enableStats();
    void reviewed(QString term, int pagenum);
    void usrsrating(int val);

    std::vector< std::vector< int > > xcursor;

private:

/*=== Private Statistics Variables ===*/

    QString * directory;
    QString title;

    QString * logFile;

    int * pageVists;
    int bookSize;

    std::vector< std::vector<double> > pageTimes;
    int index;
    time_t start;

    bool disable_flag;

/*=== Private Statistics Functions ===*/

    void loadStatsDocument();
    void generateStatsDocument();
    void openJournal();
    void closeJournal();

};

#endif // STATISTICS_H
