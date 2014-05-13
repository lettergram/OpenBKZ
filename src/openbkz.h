#ifndef OPENBKZ_H
#define OPENBKZ_H

#include "statsviewer/statsviewer.h"

#include <QMainWindow>
#include <QKeyEvent>

namespace Ui {
class OpenBKZ;
}

class OpenBKZ : public QMainWindow
{
    Q_OBJECT

public:
    explicit OpenBKZ(QWidget *parent = 0);
    ~OpenBKZ();

private slots:

    void on_nextButton_clicked();
    void on_prevButton_clicked();
    void on_saveBookButton_clicked();

    void on_lineEdit_page_textEdited(const QString &arg1);
    void on_toolButton_clicked();
    void on_enable_stats_button_clicked();
    void on_pushNoteButton_clicked();

    void on_search_type_currentIndexChanged(const QString &arg1);
    void on_user_rating_currentIndexChanged(int index);
    void keyPressEvent( QKeyEvent *k );

    void on_viewStats_clicked();

    void on_graphicsView_rubberBandChanged(const QRect &viewportRect, const QPointF &fromScenePoint, const QPointF &toScenePoint);

    void on_fontSizeSlider_valueChanged(int value);
    void on_thumbPage_pressed();


private:

    Ui::OpenBKZ *ui;
    statsviewer * sv;

/* Variables related to note/search */
    int fontsize;
    int start;
    int end;
    int start_search;
    int end_search;

/* Variables related to various constructs */
    current_book * book;
    library * lib;
    QString * lib_loc;
    statistics * stats;
    std::vector< QString > highlight;
    QString search;

/* Functions related to book parsing */
    void loadpage();
    void loadNewBook();
    int parseImage(int, QString, QGraphicsScene *);

/* Functions related to notes and search */
    void saveHighlightedSection();
    void searchWord();
    void searchMenu(QString);
};

#endif // OPENBKZ_H
