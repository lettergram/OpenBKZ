Officially hosted from the [Synaptitude github repository](https://github.com/Synaptitude/OpenBKZ)!

OpenBKZ
============

A book reader written in C++ with the Qt 5.2 (Qt 4+ should work) framework

Required: <a href="http://qt-project.org/downloads">Qt 4.+</a> and C++11 

Project Page/Website: <a href="http://austingwalters.com/OpenBKZ/">austingwalters.com</a>

Excellent Book Source: <a href="http://www.gutenberg.org/">Project Gutenberg</a> <i>(gutenberg.org)</i>

Platform: Cross-Platform

<h2>Purpose:</h2>

Originally dubbed basicbookreader, I intended OpenbBKZ as a template for other book readers, the goal specifically in mind
is adding a large number of analytics other book readers did not offer (or at least did not let me control).
This reader only took ~250 hours to complete at this point and I there are many bugs. Please feel free to offer suggestions or make pull requests, etc. The reason I am making this open source is the hope that someone can use it, if
they ever stumble upon my github that is.

If anyone does end up using it, I would hope you contribute to the project, or at least let me know what you use it for! It's nice to know when a project is being put to use.

<h2>Name Change:</h2>

Why the name change from BasicBookReader to OpenBKZ?

Well, I intend to market this product as an opensource alternative to many Ebook readers out there. Naming the Ebook reader OpenBKZ completes this task better than BasicBookReader could because it both identifies it as an open product and is also pretty witty in my opinion "Open Books." 

Further, I intend to launch a similar closed source project called AlphaBKZ which will provide a larger range of statistics, hopefully will attract authors, and will fund the server costs for OpenBKZ. I do not mind having an open source alternative and believe it is important, but starting a business which deals with the distrabution of information costs money.

<h2>Directions:</h2>

  <h3>Build - Using Qt Creator:</h3>
  <ul>
    <li>Download OpenBKZ</li>
    <li>Install Qt 5+ </li>
    <li>Open Qt Creator</li>
    <li>Click File->Open file or project</li>
    <li>Select the ".pro" file</li>
    <li>Click the big Green Arrow and run your program (this should build it)</li>
  </ul>
  <h3>Build - Linux: </h3>
  <ul>
    <li>Download OpenBKZ (either with git clone or by download the zip)</li>
    <li>Run "make"</li>
    <li>Run openbkz with "./openbkz" in the release folder</li>
  </ul>
  <h3>Run:</h3>
    <ul>
      <li>**Important** - If you decide to build and run a"release" version of Qt, 
      make sure that your books folder is inside folder. Alternatively, you could copy
      the books folder to the parent directory, OpenBKZ will search the parent directory
      for "/books" as well.</li>
      <li>Launch the program once your books folder is set up correctly, by default you have Tom Sawyer.
      If you would like to launch a different book replace the name with the file you wish to load.</li>
      <ul>
        <li>At the moment only .txt files are supported (Project Gutenberg has .txt files free to download)</li>
        <li>Click add to library to add it to your library.</li>
      </ul>
    <li>Once the book is added to the library if you would like to switch to your old book you can:</li>
      <ul>
        <li>Replace the current book name with the book you wish to switch to (make sure to bookmark if you want to 
        keep your page). </li>
        <li>Once you replace the name click "checkout book" and you will checkout the book from the library.</li>
      </ul>
    </ul>  
  <h3>Features:</h3>
     <ul> 
    <li>**Saved book locations**, the first load is slow. This is because we index all of the pages. After the first
      time you load a book it is saved in a file. Since I decided to keep this as lean as possible every time you
      launch the application it will load library.txt which will contain all the titles you currently have, plus 
      all indexes. This means that when you launch a book it should load instantly (since we only load one page at 
      a time - 50 lines).</li>
    <li>**Awesome Search Feature**, just swipe acorss (or highlight with the mouse) the word/phrase you would like to
        search. With instant lookup, knowledge is at your fingertips.</li>
    <li>**Ease of use**, I intended this application to be as easy to use and as portible as possible. I didn't want 
      to have to worry about any complexities with file formats or notes. This design decision means you cannot 
      use  many features you may be use to with your current ebook readers. However, it also means I developed an 
      easily expandable and usable application, which other programmers can use as a templete to build a better 
      ebook reader. Further, something this simple is easy to maintain, repair and hard to break. I would rather 
      do one thing correcly than a lot poorly.</li>
    <li>**Data, data, data**, because I leave the hood open and all of the data is open source, you have access to
      all available data and can build libraries for better analytics. This was and is my intended use.</li>
    </ul> 
  <h3>Future Features:</h3>
    <ul>
      <li>**Pleasant UI**, as it sounds, I would like this Ebook reader to have an intuitive and amazing UI.</li>
      <li>**In app dictionary**, I am currently working on an in app dictionary which will be used to better search terms,
            but will also cover use cases, when they were first used, and more.</li>
      <li>**Online backup**, similar to Anki cards it would be nice to have an online backup system as well as a feature to
            download new books (not upload books in order to avoid copyright issues).</li>
      <li>**User specific accounts**, allowing online backups require user specific accounts. This means that people can 
            login on the same computer, but have different libraries.</li>
      <li>**Publishing**, enabling a person to self publish and sell their books through this application would be a great 
            improvement.</li>
    </ul>


Copyright Austin Walters 2014 - 2015, All-Rights Reserved
