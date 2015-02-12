#ifndef STATSVIEWER_H
#define STATSVIEWER_H

#include <QWidget>
#include <QStringList>
#include <QGraphicsScene>
#include <QGraphicsPathItem>
#include <QPointF>

#include <vector>
#include <utility>

#include "../library.h"
#include "../statistics.h"

namespace Ui {
class statsviewer;
}

class statsviewer : public QWidget
{
    Q_OBJECT

public:
    explicit statsviewer(QWidget *parent = 0);

    void initUsers(QStringList);
    void initTitle(QStringList);
    bool viewportEvent(QEvent *event);

    ~statsviewer();

    QTextStream openFile();
private slots:
    void on_titleBox_activated(const QString &arg1);

    void on_timesToggle_clicked();

    void on_statsTypeBox_activated(const QString &arg1);

    void on_zoomSlider_valueChanged(int value);

    void on_stretchSlider_valueChanged(int value);

private:

    std::vector< std::pair<QString, int > > datesRead; // Stores time read and number of pages
    Ui::statsviewer *ui;

    bool dateflag;
    bool flipflag;      // Makes the values flip over each node as printing out

    QString bookfile;
    QString statspath;

    QString toggleOp1;
    QString toggleOp2;
    int stretch;

    void updateAvgVal();

    void generateGraph();
    void generateLifeLogGraph();
    void generateLifeRatings();

    void statsParser(QString title);
    QString logParser(QString title);
    void ratingParser(QString title, bool allTitles);
};

#endif // STATSVIEWER_H
