#ifndef LIBRARY_H
#define LIBRARY_H

#include <QFile>
#include <QTextStream>
#include <QString>
#include <QMessageBox>
#include <QList>
#include <QStringList>
#include <QDir>
#include <QProgressDialog>

static int LINESPERPAGE = 33;
static int liblimit = 50;

typedef struct _current_book{

    bool open;
    const QString * file_location;
    const QString * title;
    int pagenum;

    QList<int> page;     // list of positions that represent pages
    QList<int> chapter;  // List of page number the term chapter appears

} current_book;

class library{

public:

    library(QString loc);
    void init_book(current_book * book);
    void loadbook(int index, current_book * book);

    void load_database(QString loc);
    void save_bookinfo_to_database(QString loc);

    void closeBook(current_book * book);
    QList<current_book> books;

private:

    void index_book(current_book * book);
};


#endif // LIBRARY_H
