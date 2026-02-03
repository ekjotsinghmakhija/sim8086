#include "mainwindow.h"
#include "tools/settings.h"
#include "tools/calculator.h"
#include "tools/converter.h"
#include <QApplication>
#include <QStatusBar> // <--- THIS WAS MISSING
#include <QStyle>     // <--- REQUIRED FOR ICONS

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    resize(1200, 800);
    setWindowTitle("SIM8086 - Virtual 8086 Emulator");

    // 1. Create Components
    QWidget *central = new QWidget(this);
    setCentralWidget(central);
    statusLabel = new QLabel("Status: Ready");
    statusBar()->addWidget(statusLabel);

    editor = new CodeEditor();
    editor->setStyleSheet("font-family: 'Consolas'; font-size: 14px;");

    // AsmHighlighter is a helper, we create it and parent it to the document
    // We don't need to store the pointer if we don't access it later
    new AsmHighlighter(editor->document());

    regView = new RegisterView();
    memView = new MemoryView();

    // 2. Initialize Managers
    uiManager = new UIManager(this);
    execManager = new ExecutionManager(&cpu, editor, regView, memView, statusLabel);
    hwController = new HardwareController(this);

    // 3. Setup Layout
    uiManager->setupLayout(central, editor, regView, memView, rightLayout);
    hwController->setupLayout(rightLayout);

    // 4. Connect Hardware
    hwController->connectToCpu(cpu);

    // 5. Setup Toolbar & Connect Actions
    QToolBar *toolbar = addToolBar("Main");
    toolbar->setMovable(false);
    toolbar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    QStyle *s = QApplication::style();

    // File Actions
    toolbar->addAction(s->standardIcon(QStyle::SP_FileIcon), "New", [this](){ uiManager->onNew(editor); });
    toolbar->addAction(s->standardIcon(QStyle::SP_DirIcon), "Open", [this](){ uiManager->onOpen(editor); });
    toolbar->addAction(s->standardIcon(QStyle::SP_DriveFDIcon), "Save", [this](){ uiManager->onSave(editor); });
    toolbar->addSeparator();

    // Execution Actions
    toolbar->addAction(s->standardIcon(QStyle::SP_BrowserReload), "Compile", execManager, &ExecutionManager::compile);
    toolbar->addSeparator();
    toolbar->addAction(s->standardIcon(QStyle::SP_ArrowLeft), "Back", execManager, &ExecutionManager::stepBack);
    toolbar->addAction(s->standardIcon(QStyle::SP_ArrowRight), "Step", execManager, &ExecutionManager::step);
    toolbar->addAction(s->standardIcon(QStyle::SP_MediaPlay), "Run", execManager, &ExecutionManager::run);
    toolbar->addAction(s->standardIcon(QStyle::SP_MediaStop), "Stop", execManager, &ExecutionManager::stop);
    toolbar->addSeparator();

    // Tools
    toolbar->addAction(s->standardIcon(QStyle::SP_ComputerIcon), "Calc", [](){ (new CalculatorDialog())->show(); });
    toolbar->addAction(s->standardIcon(QStyle::SP_FileDialogDetailedView), "Conv", [](){ (new ConverterDialog())->show(); });
    toolbar->addAction(s->standardIcon(QStyle::SP_FileDialogListView), "Settings", [this](){
        SettingsDialog dlg(this);
        connect(&dlg, &SettingsDialog::settingsChanged, this, &MainWindow::applySettings);
        dlg.exec();
    });

    // 6. Final Polish
    uiManager->setupAutocomplete(editor);
    applySettings();
}

void MainWindow::applySettings() {
    QFont font = editor->font();
    font.setPointSize(SettingsDialog::getFontSize());
    editor->setFont(font);

    if (SettingsDialog::isDarkMode()) {
        qApp->setStyleSheet("QMainWindow { background-color: #2D2D30; color: #CCC; } QPlainTextEdit { background: #1E1E1E; color: #D4D4D4; }");
    } else {
        qApp->setStyleSheet("");
    }
}
