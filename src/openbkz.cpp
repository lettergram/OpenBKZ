
/** Local Headers **/
#include "openbkz.h"
#include "ui_openbkz.h"
#include "library.h"
#include "statistics.h"

/** C++ and Qt **/
#include <iostream>
#include <QFileDialog>
#include <QInputDialog>
#include <QDialogButtonBox>
#include <QCursor>
#include <QPoint>
#include <QDesktopServices>
#include <QMovie>

/**
 *  Constructor of the OpenBKZ class
 *
 * @brief OpenBKZ::OpenBKZ - creates a qt window to display the books
 * @param parent - The qt widget object
 */
OpenBKZ::OpenBKZ(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::OpenBKZ){

        this->grabKeyboard();
        ui->setupUi(this);

        QDir dir(QApplication::applicationDirPath());

        int i = 0;
        while(!dir.cd("books")){
            dir.cdUp();
            if(i++ == 5){ break; }
        }

        // Initialize Library/Book
        this->lib_loc = new QString(dir.absolutePath() + "/");
        this->lib = new library(*this->lib_loc);
        this->book = new current_book();
        this->stats = NULL;
        this->book->open = false;
        this->book->file_location = new QString(*this->lib_loc);

        // Initialize UI Items
        this->fontsize = 13;
        this->start = 0;
        this->end = 0;
        this->search = QString("Pages");
        this->grabKeyboard();
        this->locateWord = false;

        // Loads OpenBKZ logo
        dir.cdUp();
        dir.cd("images");
        QGraphicsScene* scene = new QGraphicsScene();
        QPixmap logo;
        logo.load(dir.absolutePath() + "/OpenBKZ-alpha.png");
        QGraphicsPixmapItem * item = new QGraphicsPixmapItem(logo);
        item->setScale(0.5);
        scene->addItem(item);
        ui->graphicsView->setScene(scene);
}

/**
 *  Public Function of the OpenBKZ class
 *
 * @brief OpenBKZ::~OpenBKZ Deconstructor, called when the ui is destroyed
 * Deletes all excess data
 */
OpenBKZ::~OpenBKZ(){

    if(book == NULL || book->page.size() < 1){ return; }

    releaseKeyboard();
    if(book->open){ lib->closeBook(book); }
  
    if(stats!= NULL){
        stats->endPage(book->pagenum);
        delete stats;
    }

    on_saveBookButton_clicked();

    delete book;
    delete ui;
}

/**
 * Public Function of the OpenBKZ class
 *
 * @brief OpenBKZ::on_nextbutton_clicked - loads next page
 */
void OpenBKZ::on_nextButton_clicked(){
    if(this->book == NULL){ return; }
    if(this->book->pagenum >= this->book->page.size() - 1){ return; }
    this->stats->endPage(this->book->pagenum);
    (this->book->pagenum)++;
    this->grabKeyboard();
    loadpage();
}

/**
 * Public Function of the OpenBKZ
 *
 * @brief OpenBKZ::on_prevButton_clicked - s previous page
 */
void OpenBKZ::on_prevButton_clicked(){

    if(this->book == NULL){ return; }
    if((this->book->pagenum) > 0){
        this->stats->endPage(this->book->pagenum);
        (this->book->pagenum)--;
    }

    this->grabKeyboard();
    loadpage();
}

/**
 * Public Function of the OpenBKZ class
 *
 * @brief OpenBKZ::on_saveBookButton_clicked - saves the book to the library
 */
void OpenBKZ::on_saveBookButton_clicked(){

    if(book == NULL || book->page.size() < 1){ return; }

    ui->saveBookButton->setText("Bookmarked!");

    for(int i = 0; i < lib->books.size(); i++)
        if(book->title->compare(lib->books[i].title, Qt::CaseInsensitive) == 0)
            lib->books[i].pagenum = book->pagenum;

    lib->save_bookinfo_to_database(*lib_loc);
}

/**
 * Public Function of the OpenBKZ class
 *
 * @brief OpenBKZ::on_lineEdit_page_textEdited - changes the book to that particular page
 * @param arg1 - the text entered converted to page number and loads
 */
void OpenBKZ::on_lineEdit_page_textEdited(const QString &arg1){

    if(book == NULL){ return; }
    this->stats->endPage((book->pagenum));

    if(arg1.toInt() > book->page.count())
        QMessageBox::information(0, "Error", "Pages out of bounds");

    if((search).compare(QString("Pages"), Qt::CaseInsensitive) == 0){
        if(arg1.toInt() > this->book->page.count() - 1){ return; }
        book->pagenum = arg1.toInt();
    }else if((search).compare(QString("Chapters"), Qt::CaseInsensitive) == 0){
        if(arg1.toInt() > this->book->chapter.count() - 1){ return; }
        book->pagenum = this->book->chapter[arg1.toInt()];
    }

    this->stats->endPage(this->book->pagenum);
    loadpage();
}

/**
 * Public Function of the OpenBKZ class
 *
 * @brief OpenBKZ::on_toolButton_clicked - if the tool button is clicked
 *      a window appears where you can choose what book to load. This function is called
 *      and will parse the file and load the book
 */
void OpenBKZ::on_toolButton_clicked(){

    if(this->stats != NULL){
        if(this->book != NULL){
            this->stats->endPage(this->book->pagenum);
        }
        delete this->stats;
    }

    if(book->open)
      this->lib->closeBook(this->book);
   // QFileDialog * menu = new QFileDialog();
    QString fileName = QFileDialog::getOpenFileName(this, "Select a file to open...", (*this->book->file_location + "/books"));

    if(fileName == NULL)
        return;

    QStringList list = fileName.split("/", QString::SkipEmptyParts);
    this->book->title = new QString(list[list.count() - 1]);
    this->book->file_location = new QString(fileName.remove((*this->book->title), Qt::CaseInsensitive));

    loadNewBook();
}

/**
 * Public Function of the OpenBKZ class
 *
 * @brief OpenBKZ::on_enable_stats_button_clicked - enables/disables statistics
 *              (this is default).
 */
void OpenBKZ::statsGathering(){
    if(this->stats== NULL){ return; }
    if(!ui->actionStatus->isChecked()){
        this->stats->enableStats();
        QMessageBox::information(0, "Statistics", "Statistics Enabled");
    }else{
        this->stats->disableStats();
        QMessageBox::information(0, "Statistics", "Statistics Disabled");
    }
}

/**
 * Private function of the OpenBKZ class
 *
 * @brief OpenBKZ::parseImage - parses a given <img src=''>
 *          and adds the image to the scene if it exists. Currently
 *          Only works for following format <img src='location of
 *          image in books directory'>
 * @param y - current y-line postion
 * @param line - the line text to search for parsable image file
 * @param scene - scene to add image too
 * @return - 0 if no image found, the number of height (in pixels) of the image
 *          if the image exists.
 */
int OpenBKZ::parseImage(int y, QString line, QGraphicsScene * scene){

    QPixmap pixel_image;

    //TODO: Add OpenBKZ private variable to hold y

    if(line.indexOf("img src=", 0, Qt::CaseInsensitive) != -1){
        QStringList imageName = line.split("\"");
        QPixmap pixel_image;

        pixel_image.load(*this->lib_loc + "images/" + imageName[1]);
        if(pixel_image.isNull())
            return 0;
        QGraphicsPixmapItem * item = new QGraphicsPixmapItem(pixel_image);
        item->setY(y);
        scene->addItem(item);
        return pixel_image.height();
    }
    return 0;
}


/**
 * Private Function of the AlphaBKZ class
 *
 * @brief OpenBKZ::next_page - loads the next page (30 lines) from the current book
 * into the window.
 *
 * TODO: Need refactoring!!!
 * TODO: Spacing is slightly off
 *
 */
void OpenBKZ::loadpage(){
  
  if(this->book == NULL || this->book->page.size() < 1){ return; }
  
  ui->saveBookButton->setText("Bookmark");
  ui->thumbPage->setText("Back to Page");
  QString str("");
  this->highlight.clear();

  
  QFile file(*book->file_location + *book->title);
  if(!file.open(QIODevice::ReadOnly))
    QMessageBox::information(0, "Error", file.errorString());
  
  
  QGraphicsScene * scene = new QGraphicsScene();
  
  QTextStream stream(&file);
  if(!stream.seek(this->book->page[this->book->pagenum])){
    stream.seek(0);
    this->book->pagenum = 0;
  }
  
  QFont f(ui->styleBox->currentText(), fontsize);
  QFont bf(ui->styleBox->currentText(), fontsize);
  
  int curline_pos = 0;
  int imgline = 0;
  int wordPosIndex = 0;
  
  for(int i = 0; i < LINESPERPAGE; i++){
    
    QString line = "";
    QString preTerm = "";
    QString boldTerm = "";
    QString postTerm = "";
    
    line.append(stream.readLine(85)); // TODO: Improve line and page parse
    
    // Currently accurate to line
    if(locateWord && (wordPos[wordPosIndex] == i)){
      
      bf.setUnderline(true);
      bf.setBold(true);
      
      QRegExp delimiters("(\\)|\\(|\\ |\\,|\\.|\\:|\\t)");
      QStringList list = line.split(delimiters, QString::KeepEmptyParts);
      bool termFound = false;
      
      preTerm.append("-> ");
      
      for(int index = 0; index < list.size(); index++){
        
        QString toadd = list[index][0] + list[index].mid(1).toLower() + " ";
        
        if(ui->lineEdit_page->text().compare(list[index], Qt::CaseInsensitive) == 0){
          boldTerm.append(toadd);
          termFound = true;
        }else if(!termFound){
          preTerm.append(toadd);
        }else{
          postTerm.append(toadd);
        }
      }
      
      wordPos[wordPosIndex] = 40;
      wordPosIndex++;
    }else{
      f.setUnderline(false);
      f.setBold(false);
      preTerm.append(line);
    }
    
    imgline = this->parseImage(curline_pos, preTerm, scene);
    if(imgline == 0){
      if((preTerm.size() + boldTerm.size() + postTerm.size()) > 84){
        scene->addText(postTerm + '-', f)->setPos(0, curline_pos);
      }else{
        scene->addText(preTerm, f)->setPos(0, curline_pos);
        int preAdj = (preTerm.size() * f.pointSize() / 2) + (2*preTerm.size() / (1+boldTerm.size()));
        scene->addText(boldTerm, bf)->setPos(preAdj, curline_pos);
        int boldAdj = (boldTerm.size()* bf.pointSize() / 2) + boldTerm.size();
        scene->addText(postTerm, f)->setPos(preAdj + boldAdj, curline_pos);
      }
    }
    curline_pos += (imgline | this->fontsize) + 3; // Not exactly inline but close enough
  }

  //scene->addText(QString("").rightJustified(135, ' '))->setPos(0, curline_pos); // Hacky solution
  ui->graphicsView->setScene(scene);

  QString curPage, maxPage;
  curPage.sprintf("%4.4d", this->book->pagenum);
  maxPage.sprintf("%4.4d", this->book->page.count());
  ui->pageLabel->setText(curPage + " / " + maxPage);

  this->stats->startPage(this->book->pagenum);
  locateWord = false; // TODO REMOVE AT SOME POINT
  wordPos.clear();
  file.close();
}


/**
 * Private Function of OpenBKZ class
 *
 * @brief OpenBKZ::loadNewBook - loads a book already in the library into memory
 * Closing the previously open book as well as the accompaning statistics
 */
void OpenBKZ::loadNewBook(){

    indexingPage();
    QMessageBox::information(0, "Congratulations!",
      "Adding a new addition to the library.\nDon't worry I'll index it for ya!\nThis process can take up to 20 seconds.");



    for(int i = 0; i < this->lib->books.count(); i++){
        if(book->title->compare(lib->books[i].title, Qt::CaseInsensitive) == 0){
            lib->loadbook(i, book);
            this->stats= new statistics(*this->book->title, this->book->page.count(), LINESPERPAGE);
            loadpage();
            return;
        }
    }

    this->book->open = true;

    this->lib->init_book(this->book);
    on_saveBookButton_clicked();
    this->stats= new statistics(*this->book->title, this->book->page.count(), LINESPERPAGE);
    loadpage();
}

/**
 * Public funcion of OpenBKZ class
 *
 * @brief OpenBKZ::on_pushNoteButton_clicked - adds a highlighted section to the "<bookname>.notes"
 * The user can then add a specific note.
 */
void OpenBKZ::on_pushNoteButton_clicked(){

    if(this->book == NULL){ return; }
    if(this->stats== NULL){ return; }
    this->releaseKeyboard();
    bool ok;
    QStringList title = (*this->book->title).split(".", QString::SkipEmptyParts);
    QString note = QInputDialog::getText(this, tr("Highlights and Notes"),
                                         tr("Note"), QLineEdit::Normal,
                                         QDir::home().dirName(), &ok);

    QDir dir(QApplication::applicationDirPath());
    int j = 0;
    while(!dir.cd("notes")){ dir.cdUp(); if(j++ == 7){ break; } }

    QString location(dir.absolutePath() + "/");
    QFile file(QString(location + title[0] + ".notes"));

    if(!file.open(QIODevice::ReadWrite | QIODevice::Append | QIODevice::Text))
        QMessageBox::information(0, "Error", "Writing in journal");

    QTextStream stream(&file);

    stream << title[0] << ", page " << this->book->pagenum << ": " << endl;
    stream << "Quote:\n\n";
    for(unsigned long i = 0; i < this->highlight.size(); i++)
        stream << "     " + this->highlight[i] + '\n';
    stream << "\n\nNote:\n" << note << endl;
    stream << "---------------------------\n" << endl;

    this->highlight.clear();
    file.close();
}

/**
 * Public function of the OpenBKZ class
 *
 * @brief OpenBKZ::on_comboBox_currentIndexChanged - This combobox is used
 *      to change what you are using the search bar for
 * @param arg1 - the item you are searching for
 */
void OpenBKZ::on_search_type_currentIndexChanged(const QString &arg1){
    if(this->book == NULL){ return; }
    search = QString(arg1);
    if((search).compare(QString("Chapters"), Qt::CaseInsensitive) == 0)
        QMessageBox::information(0, "Experimental", "This is experimental and may not work");
    if((search).compare(QString("Term"), Qt::CaseInsensitive) == 0)
        QMessageBox::information(0, "Experimental", "This is experimental, currently one finds next use of the term!");
    try{
     this->releaseKeyboard();
    }catch(int e){
        std::cout << "error" << std::endl;
    }
}

/**
 * Public function of the OpenBKZ class
 *
 * @brief OpenBKZ::on_user_rating_currentIndexChanged - This combobox
 *      is used to select how many stars a user would rate the book
 * @param index - the number of stars the user would give a book
 */
void OpenBKZ::on_user_rating_currentIndexChanged(int index){

    if(this->book == NULL){ return; }
    this->stats->usrsrating(index);
}

/**
 * Public function of the OpenBKZ Class
 *
 * @brief OpenBKZ::on_viewStats_clicked - if
 *      statistics is clicked open the view statistics window
 */
void OpenBKZ::on_viewStats_clicked(){

    sv = new statsviewer();
    QStringList titles;
    for(int i = 0; i < this->lib->books.size(); i++)
        titles.append(*this->lib->books[i].title);
    sv->initTitle(titles);
    sv->show();
}

/**
 * Private Slot of the OpenBKZ
 *
 * @brief OpenBKZ::on_graphicsView_rubberBandChanged - Saves the position
 *          the start and end point of the highlightes section
 * @param viewportRect - Current viewport rectangle - NOT USED
 * @param fromScenePoint - end point of highlighted box
 * @param toScenePoint - start point of highlighted box
 */
void OpenBKZ::on_graphicsView_rubberBandChanged(const QRect &viewportRect, const QPointF &fromScenePoint, const QPointF &toScenePoint){

    if(book == NULL || book->page.size() < 1){ return; }
    if(fromScenePoint.y() != 0 && toScenePoint.y() != 0){

        this->start = toScenePoint.y();
        this->end = fromScenePoint.y();

        if(this->start/(this->fontsize + 3) == this->end/(this->fontsize + 3)){
            this->start_search = fromScenePoint.x();
            this->end_search = toScenePoint.x();
        }
        return;
    }

    if(this->start/(this->fontsize + 3) == this->end/(this->fontsize + 3))
        this->searchWord();
    else
        this->saveHighlightedSection();
}

/**
 * Private function of OpenBKZ
 *
 * TODO: Add internal dictinary/other lookup for words,
 *       display them and ask if they would like to Google.
 *
 * @brief OpenBKZ::searchMenu - Opens the searched term, adjusts
 *          for any form of error.
 * @param line - the line which is being searched
 */
void OpenBKZ::searchMenu(QString line){

    QString copy(line);

    int space = line.size() / (2 * fontsize);
    line.remove(this->end_search + space, 85);
    line.remove(0, this->start_search - space/2);

    QStringList term = line.split(' ', QString::SkipEmptyParts);
    QStringList words = copy.split(' ', QString::SkipEmptyParts);
    line.clear();

    for(int j = 0; j < term.size(); j++){
        for(int i = 0; i < words.size(); i++){
            if(words[i].contains(term[j]) && term[j].size() > (words[i].size() / 2) ){
                line.append(words[i] + ' ');
                break;
            }
        }
    }

    if(line.size() > 0)
        QDesktopServices::openUrl(QUrl("https://www.google.com/#q=" + line));
}

/**
 * Private Function of OpenBKZ
 *
 * @brief OpenBKZ::searchWord - opens a google webpage with yoursearch
 *      it's awful, but it works (kinda)...
 *
 *  You just highlight the word(s) in the line
 *
 * FOR THE FUTURE: I will make it so if you click shift,
 *      all of the words will highlight so you can pick a specific word
 */
void OpenBKZ::searchWord(){

    QFile file(*book->file_location + *book->title);
    if(!file.open(QIODevice::ReadOnly))
        QMessageBox::information(0, "Error", file.errorString());
    QTextStream stream(&file);
    stream.seek(this->book->page[this->book->pagenum]);

    if(this->end_search < this->start_search){
        int temp = this->start_search;
        this->start_search = this->end_search;
        this->end_search = temp;
    }

    this->start_search /= (this->fontsize / 2);
    this->end_search /= (this->fontsize / 2);
    QString line;

    for(int i = 0; i < LINESPERPAGE; i++){
        line = stream.readLine(85);
        if(this->start / (this->fontsize + 3) == i){
            this->searchMenu(line);
            break;
        }
    }

    this->stats->reviewed(line, this->book->pagenum);
}

/**
 * Private Function of OpenBKZ
 *
 *
 * WARNING: CURRENTLY INACCURATE ON PAGES WITH IMAGES - CRITICAL FIX COMING SOON
 *
 *
 * @brief OpenBKZ::saveHighlightedSection - Saves the highlighted
 *      section in the OpenBKZs qscene into this->heighlight[i]
 */
void OpenBKZ::saveHighlightedSection(){

    QFile file(*book->file_location + *book->title);
    if(!file.open(QIODevice::ReadOnly))
        QMessageBox::information(0, "Error", file.errorString());
    QTextStream stream(&file);
    stream.seek(this->book->page[this->book->pagenum]);

    if(this->end < this->start){
        int temp = this->start;
        this->start = this->end;
        this->end = temp;
    }

    this->start /= (this->fontsize + 3);
    this->end /= (this->fontsize + 3);

    /* Iterate to start */
    for(int i = 0; i < this->start; i++){ stream.readLine(85); }

    /* Add to highlighted section */
    for(int i = this->start; i < this->end; i++)
        this->highlight.push_back(stream.readLine(85));
}


/**
 * Private Slot of the OpenBKZ class
 *
 * @brief OpenBKZ::on_fontSizeSlider_valueChanged - changes the fontsize.
 * @param value - the font size they would rather have.
 */
void OpenBKZ::on_fontSizeSlider_valueChanged(int value){

    this->fontsize = value;
    loadpage();
}

/**
 * Private Slot of the OpenBKZ class
 *
 * @brief OpenBKZ::on_thumbPage_pressed - Returns user
 *          to the bookmarked page.
 */
void OpenBKZ::on_thumbPage_pressed(){

    if(this->book == NULL){ return; }

    for(int i = 0; i < lib->books.size(); i++){
        if(0 == lib->books[i].title->compare(*(book->title), Qt::CaseInsensitive)){
            if(0 == ui->thumbPage->text().compare("Back to Page", Qt::CaseInsensitive)){
                book->prevPage = book->pagenum;
                book->pagenum = lib->books[i].pagenum;
            }else{
                int tmp = book->pagenum;
                book->pagenum = book->prevPage;
                book->prevPage = tmp;
            }
        }
    }

    loadpage();
    ui->thumbPage->setText("Whoops!");
}

/**
 * Public function of the OpenBKZ class
 *
 * @brief OpenBKZ::keyPressEvent - is called when grabKeyboard()
 *      is in effect. It will determine which key has been pressed and call that function
 * @param k - the key pressed.
 */
void OpenBKZ::keyPressEvent( QKeyEvent *k ){

    if(k->key() == Qt::Key_Right)
        on_nextButton_clicked();
    else if(k->key() == Qt::Key_Left)
        on_prevButton_clicked();
    else if(k->key() == Qt::Key_Backspace)
        this->releaseKeyboard();
    else if(k->key() == Qt::Key_O)
        on_toolButton_clicked();
    else if(k->key() == Qt::Key_B)
        on_saveBookButton_clicked();
    else if(k->key() == Qt::Key_S)
        on_viewStats_clicked();
    else if(k->key() == Qt::Key_R)
        on_thumbPage_pressed();
    if(k->key() == Qt::Key_0
       || k->key() == Qt::Key_1
       || k->key() == Qt::Key_2
       || k->key() == Qt::Key_3
       || k->key() == Qt::Key_4
       || k->key() == Qt::Key_5
       || k->key() == Qt::Key_6
       || k->key() == Qt::Key_7
       || k->key() == Qt::Key_8
       || k->key() == Qt::Key_9){
        this->releaseKeyboard();
    }
}

/**
 * @brief OpenBKZ::loadingPage - provides a loading page for indexing
 */
void OpenBKZ::indexingPage(){

    QGraphicsScene * scene = new  QGraphicsScene();
    QDir dir = *lib_loc;
    dir.cdUp();
    dir.cd("images");
    QLabel *gif_anim = new QLabel();
    QMovie *movie = new QMovie(dir.absolutePath() + "/loader.gif");
    gif_anim->setMovie(movie);
    movie->start();
    scene->addWidget(gif_anim);
    ui->graphicsView->setScene(scene);
}

/**
 * @brief OpenBKZ::on_styleBox_activated - When the font is changed,
 *          this function is called, reload the page with the index.
 * @param index - not used, but is the index of the font selected
 */
void OpenBKZ::on_styleBox_activated(int index){
    loadpage();
}

/* When the window is resized this function is called */
void OpenBKZ::resizeEvent(QResizeEvent* event){

    double ratio = double(this->height()) / double(ui->fontSizeSlider->value() * LINESPERPAGE);
    int count = 0;

    while(ratio > 1.4 || ratio < 1.32){
        if(ratio > 1.32){
            ui->fontSizeSlider->setValue(ui->fontSizeSlider->value() + 1);
        }else if(ratio < 1.4){
            ui->fontSizeSlider->setValue(ui->fontSizeSlider->value() - 1);
        }
        if(count == 10){
            break;
        }else{
            count++;
            ratio = double(this->height()) / double(ui->fontSizeSlider->value() * LINESPERPAGE);
        }
    }
}

/**
 * @brief OpenBKZ::on_lineEdit_page_returnPressed - If search is set to "Term"
 *                                                  it searches for the next term.
 *
 * WARNING will be updated for the future
 *
 */
void OpenBKZ::on_lineEdit_page_returnPressed(){

    QString searchTerm = ui->lineEdit_page->text();
    QList<QString> termLoc;

    if(this->search.compare("Term", Qt::CaseInsensitive) == 0){
      
        ui->lineEdit_page->setText("Searching..");
        termLoc = lib->searchTerm(searchTerm, book);  // TODO: CURRENTLY JUST STOPS AT FIRST FOUND TERM
        ui->lineEdit_page->setText(searchTerm);
    }else{
        on_lineEdit_page_textEdited(searchTerm);
        return;
    }

    wordPos.resize(termLoc.size());

    // TODO: Should make this a combo box or easy selection (currently not)
    for(int i = termLoc.size() - 1; i >= 0; i--){

        QStringList list = termLoc[i].split(",", QString::SkipEmptyParts);
        book->pagenum = list[0].toInt();    // page number
        // list[1].toInt();                 // location in file
        wordPos[i] = list[2].toInt();       // line number
        locateWord = true;
        loadpage();
    }

    std::reverse(wordPos.end(), wordPos.begin());
}

void OpenBKZ::on_lineEdit_page_selectionChanged(){
    this->releaseKeyboard();
}
