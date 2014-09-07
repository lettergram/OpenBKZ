#include "library.h"
#include <iostream>

/**
 * Constructor for the library class
 */
library::library(QString loc){
    load_database(loc);
}

/**
 * Public Function of the library class
 *
 * @brief library::load_database - loads the data from /books/library.db
 * @param lib_loc - the library location, it will search all parent directories
 */
void library::load_database(QString lib_loc){

    QFile file(lib_loc + QString("library.db"));
    if(!file.open(QIODevice::ReadWrite))
        QMessageBox::information(0, "Error", file.errorString());
    QTextStream database(&file);

    while(!database.atEnd()){
        current_book b;
        b.title = new QString(database.readLine());
        b.file_location = new QString(database.readLine());

        QString open(*b.file_location);
        open.append(*b.title);

        b.pagenum = database.readLine().toInt();

        QString chapters = database.readLine();
        QStringList list = chapters.split(",", QString::SkipEmptyParts);
        for(int i = 0; i < list.count(); i++)
            b.chapter.append(list[i].toInt());

        QString pages = database.readLine();
        list = pages.split(",", QString::SkipEmptyParts);
        for(int i = 0; i < list.count(); i++)
            b.page.append(list[i].toInt());

        this->books.append(b);
    }
    file.close();
}

/**
 * Public Function of the library class
 *
 * @brief library::loadbook - loads a particular book from the this->books
 *        QList (aka the library), it will load the book into *book and the display
 * @param index - The index of the book in the list
 * @param book - The pointer to the current book to be displayed
 */
void library::loadbook(int index, current_book * book){

    book->pagenum = this->books[index].pagenum;
    book->title = this->books[index].title;
    book->file_location = this->books[index].file_location;

    for(int i = 0; i < this->books[index].chapter.count(); i++)
        book->chapter.append(this->books[index].chapter[i]);

    for(int i = 0; i < this->books[index].page.count(); i++)
        book->page.append(this->books[index].page[i]);

    QFile file(*book->file_location + *book->title);
    if(!file.open(QIODevice::ReadOnly))
        QMessageBox::information(0, "Error", file.errorString());

    QTextStream stream(&file);

    if(!stream.seek(book->page.value(book->pagenum))){
        stream.seek(0);
        (book->pagenum) = 0;
    }

    book->open = true;
    file.close();
}

/**
 * Public Function of the library class
 *
 * @brief library::save_bookinfo_to_database - Saves the current book data to
 *          books/library.db, which will be loaded on startup.
 * @param lib_loc - the locaiton of library.db in the file system
 */
void library::save_bookinfo_to_database(QString lib_loc){

    QFile file(lib_loc + QString("library.db"));

    if(!file.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text))
        QMessageBox::information(0, "Error", file.errorString());

    QTextStream database(&file);

    int start = 0;
    if(books.count() == liblimit)
        start = 20;

    for(int i = start; i < books.count(); i++){
        database << *books[i].title << "\n";
        database << *books[i].file_location << "\n";
        database << books[i].pagenum << "\n";

        for(int j = 0; j < books[i].chapter.count(); j++)
            database << QString::number(books[i].chapter.value(j)) << ",";
        database << "\n";

        for(int j = 0; j < books[i].page.count(); j++)
            database << QString::number(books[i].page.value(j)) << ",";
        database << "\n";
    }
    file.close();
}

/**
 * Public Function of the library class
 *
 * @brief index_book - This function is a helper function which indexes the books pages
 * @param book - The book which needs an index
 */
void library::index_book(current_book* book){

    QFile file(*book->file_location + *book->title);
    if(!file.open(QIODevice::ReadOnly))
        QMessageBox::information(0, "Error", file.errorString());
    QTextStream stream(&file);

    book->page.push_back(0);
    book->chapter.push_back(0);

    int previousChapter = 0;

    for(int page = 0; !stream.atEnd(); page++){
        for(int i = 0; i < LINESPERPAGE; i++){
            QString check = stream.readLine(85);
            QStringList words = check.split(" ", QString::SkipEmptyParts);
            for(int j = 0; j < words.count(); j++){
                if(words[j].compare(QString("Chapter"), Qt::CaseInsensitive) == 0
                   || words[j].compare(QString("Session"), Qt::CaseInsensitive) == 0){
                    if(previousChapter + 1 < page)
                        book->chapter.push_back(page);
                    previousChapter = page;
                }
            }
        }
        book->page.push_back(stream.pos());
    }
    file.close();
}

/**
 * Public Function of the Library Class
 *
 * TODO: STILL EXPERIMENTING, JUST STOPS AT FIRST FOUND TERM
 *
 * @brief search - searches for terms throughout the current book
 * @param term - term to search for
 * @param book - book to search
 * @return list of <String = "page number,stream position"> the term is located on
 */
QList<QString> library::searchTerm(QString term, current_book * book){

    QFile file(*book->file_location + *book->title);
    if(!file.open(QIODevice::ReadOnly))
        QMessageBox::information(0, "Error", file.errorString());
    QTextStream stream(&file);

    QList<QString> termLoc;

    bool found = false; // TODO: REMOVE
    int page = 0;

    /* Go to proper section */
    for(page = 0; page < book->pagenum; page++)
        for(int i = 0; i < LINESPERPAGE; i++)
            stream.readLine(85);

    /* Search book from current page */
    for(page; !stream.atEnd(); page++){
        for(int i = 0; i < LINESPERPAGE; i++){
            QString check = stream.readLine(85);
            QStringList words = check.split(" ", QString::SkipEmptyParts);
            for(int j = 0; j < words.count(); j++){
                if(words[j].compare(term, Qt::CaseInsensitive) == 0){
                    termLoc << QString::number(page) + "," + QString::number(stream.pos()) + "," + QString::number(i);
                    found = true;
                }
            }
        }
        if(found){ break; }
    }
    file.close();
    return termLoc;
}


/**
 * Public Function of the library class
 *
 * @brief library::closeBook - use if you are attempted to grab a new book,
 *          closes current book.
 * @param book - book to be closed
 */
void library::closeBook(current_book* book){
    if(book == NULL || book->file_location->size() == 0)
        return;
    for(int i = 0; i < this->books.size(); i++){
        if(this->books[i].title->compare(*book->title, Qt::CaseInsensitive) == 0){
            this->books[i].pagenum = book->pagenum;
            break;
        }
    }
    book->open = false;
    book->page.clear();
}

/**
 * Public Function of the library class
 *
 * @brief init_book - This function is function used to initialize the book struct
 * @param book - The book which needs to be initalized
 */
void library::init_book(current_book * book){

    if(book == NULL){ return; }

    for(int i = 0; i < this->books.count(); i++){
        if(book->title->compare(this->books[i].title, Qt::CaseInsensitive) == 0){
            loadbook(i, book);
            return;
        }
    }

    QString open(*book->file_location);
    open.append(*book->title);

    QFile file(*book->file_location + *book->title);
    if(!file.open(QIODevice::ReadOnly)){
        QMessageBox::information(0, "Error", "No Book Found");
        return;
    }

    book->pagenum = 0;
    index_book(book);
    books.append(*book);
    file.close();
}
