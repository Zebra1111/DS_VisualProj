#include "mainScene.h"
#include "ui_mainScene.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    initWindow();
    createMainContainer();
    createCloseButton();
    createTreeWidget();
    createVisualizationArea();
    createCodeAndButtonArea();
    setupLayout();

    startGDBProcess();

    connect(treeWidget, &QTreeWidget::itemClicked, this, &Widget::onTreeItemClicked);
    connect(buildButton, &QToolButton::clicked, this, &Widget::compileCode);
    connect(runPauseButton, &QToolButton::clicked, this, &Widget::onRunPauseButtonClicked);
    connect(stepButton, &QToolButton::clicked, [=]() {
        if (gdbProcess && gdbProcess->state() == QProcess::Running)
            gdbProcess->write("step\n");
    });

}

void Widget::initWindow()
{
    this->setWindowTitle("数据结构与算法可视化平台");
    this->setFixedSize(1440, 970);
    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground);
}

void Widget::createMainContainer()
{
    // 主体容器
    mainContainer = new QWidget(this);
    mainContainer->setStyleSheet("background-color:#2B2B2B; border-radius:13px;");

    // 阴影效果
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(20);
    shadow->setColor(QColor(0, 0, 0, 50));
    shadow->setOffset(0, 0);
    mainContainer->setGraphicsEffect(shadow);
}

void Widget::createCloseButton()
{
    QToolButton *closeButton = new QToolButton(this);
    closeButton->setText("×");
    closeButton->setStyleSheet(
        "QToolButton {color:#E0E0E0; background-color:transparent; font-size:24px;}"
        "QToolButton:hover {color:#ff5c5c;}"
    );
    closeButton->setFixedSize(30, 30);
    closeButton->move(this->width() - closeButton->width() - 30, 30);
    connect(closeButton, &QToolButton::clicked, this, &QWidget::close);
}

void Widget::createTreeWidget()
{
    // 左侧导航栏
    treeWidget = new QTreeWidget;
    treeWidget->setHeaderHidden(true);
    treeWidget->setStyleSheet(R"(
        QTreeWidget {
            border: none;
            padding: 13px;
            background-color:#333333;
            color:#E0E0E0;
            font-family:'黑体';
            font-size:17px;
        }
        QTreeWidget::item {
            outline: none; /* 移除默认的焦点边框 */
        }
        QTreeWidget::item:hover {
            background-color: #707070;
            color: #ffffff;
            border-radius: 5px;
        }
        QTreeWidget::item:selected {
            background-color: #ff2d51;
            color: #ffffff;
            border-radius: 5px;
        }
        QTreeWidget:focus {
            outline: none; /* 彻底移除焦点框 */
            border: none;
        }
    )");

    QStringList categories = {"线性表", "树", "图", "查找", "排序"};
    QStringList linearList = {" 链表", " 栈", " 队列"};
    QStringList treeList = {" 先序遍历", " 中序遍历", " 后序遍历", " 平衡二叉树", " B树"};
    QStringList graphList = {" 深度优先搜索", " 宽度优先搜索", " Dijkstra", " Floyd"};
    QStringList searchList = {" 折半查找"};
    QStringList sortList = {" 冒泡排序", " 快速排序", " 堆排序", " 桶排序"};

    // 创建父节点并设置不可选中
    QTreeWidgetItem *item1 = new QTreeWidgetItem(treeWidget, QStringList(categories[0]));
    item1->setFlags(item1->flags() & ~Qt::ItemIsSelectable);  // 移除可选标志
    for (const auto &item : linearList)
        item1->addChild(new QTreeWidgetItem(QStringList(item)));

    QTreeWidgetItem *item2 = new QTreeWidgetItem(treeWidget, QStringList(categories[1]));
    item2->setFlags(item2->flags() & ~Qt::ItemIsSelectable);
    for (const auto &item : treeList)
        item2->addChild(new QTreeWidgetItem(QStringList(item)));

    QTreeWidgetItem *item3 = new QTreeWidgetItem(treeWidget, QStringList(categories[2]));
    item3->setFlags(item3->flags() & ~Qt::ItemIsSelectable);
    for (const auto &item : graphList)
        item3->addChild(new QTreeWidgetItem(QStringList(item)));

    QTreeWidgetItem *item4 = new QTreeWidgetItem(treeWidget, QStringList(categories[3]));
    item4->setFlags(item4->flags() & ~Qt::ItemIsSelectable);
    for (const auto &item : searchList)
        item4->addChild(new QTreeWidgetItem(QStringList(item)));

    QTreeWidgetItem *item5 = new QTreeWidgetItem(treeWidget, QStringList(categories[4]));
    item5->setFlags(item5->flags() & ~Qt::ItemIsSelectable);
    for (const auto &item : sortList)
        item5->addChild(new QTreeWidgetItem(QStringList(item)));

    treeWidget->expandAll();
    treeWidget->setMaximumWidth(230);
}

void Widget::createVisualizationArea()
{
    visualAreaScene = new QGraphicsScene(this);
    visualAreaView = new GraphicsView();
    visualAreaView->setScene(visualAreaScene);
    visualAreaView->setRenderHint(QPainter::Antialiasing); // 开启抗锯齿效果
    visualAreaView->setStyleSheet("background-color:#3C3F41; border-radius:10px; margin:5px;");

    logArea = new QPlainTextEdit;
    logArea->setPlaceholderText("输出日志...");
    logArea->setReadOnly(true);
    logArea->setStyleSheet(
        "background-color:#333333; color:#ffffff; border-radius:10px; margin:5px; padding:5px;");

    verticalSplitter = new QSplitter(Qt::Vertical);
    verticalSplitter->addWidget(visualAreaView);
    verticalSplitter->addWidget(logArea);
    verticalSplitter->setStretchFactor(0, 3);
    verticalSplitter->setStretchFactor(1, 1);
    verticalSplitter->setSizes({600, 200});
}

void Widget::createCodeAndButtonArea()
{
    // 右侧代码区域布局
    codeLayout = new QVBoxLayout;

    // 代码编辑区域
    codeArea = new QTextEdit;
    codeArea->setPlainText("// 代码区域\nvoid example() {\n    // TODO: 实现示例代码\n}");
    codeArea->setMinimumWidth(300);
    codeArea->setMaximumWidth(550);
    codeArea->setStyleSheet(
            "background-color:#252526;"
            "color:#ABB2BF;"
            "border-radius:8px;"
            "margin:5px;"
            "padding:10px;"
            "font-family:'Consolas';"
        );

    // 设置 One Dark Pro 主题的字体
    QFont font;
    font.setFamily("Fira Code"); // 首选 Fira Code
    font.setPointSize(10);       // 代码字体大小
    font.setLetterSpacing(QFont::AbsoluteSpacing, 0.5);
    codeArea->setFont(font);
    codeArea->setLineWrapMode(QTextEdit::NoWrap); // 不自动换行


    // 按钮区（构建、运行、单步调试）
    QWidget *controlPanel = new QWidget;
    controlPanel->setFixedHeight(50);
    controlPanel->setMaximumWidth(550);
    controlPanel->setStyleSheet("background-color:#333333; border-radius:10px; padding:5px;");

    QHBoxLayout *controlLayout = new QHBoxLayout(controlPanel);
    controlLayout->setSpacing(15);
    controlLayout->setContentsMargins(10, 5, 10, 5);

    // 按钮样式
    QString buttonStyle = R"(
        QToolButton {
            background-color: transparent;
            border: none;
            padding: 5px;
        }
        QToolButton:hover {
            background-color: rgba(255, 255, 255, 30);
        }
    )";

    // 创建按钮
    buildButton = new QToolButton;
    buildButton->setIcon(QIcon(":/icons/build.png"));
    buildButton->setIconSize(QSize(32, 32));
    buildButton->setStyleSheet(buttonStyle);
    buildButton->setToolTip("构建");

    runPauseButton = new QToolButton;
    QIcon runIcon(":/icons/run.png");
    QIcon pauseIcon(":/icons/pause.png");

    runPauseButton->setIcon(runIcon);
    runPauseButton->setIconSize(QSize(32, 32));
    runPauseButton->setStyleSheet(buttonStyle);
    runPauseButton->setToolTip("运行");

    executionRunning = false;
    connect(runPauseButton, &QToolButton::clicked, [=]() mutable {
        executionRunning = !executionRunning;
        if (executionRunning) {
            runPauseButton->setIcon(pauseIcon);
            runPauseButton->setToolTip("暂停");
        } else {
            runPauseButton->setIcon(runIcon);
            runPauseButton->setToolTip("运行");
        }
    });

    stepButton = new QToolButton;
    stepButton->setIcon(QIcon(":/icons/step.png"));
    stepButton->setIconSize(QSize(32, 32));
    stepButton->setStyleSheet(buttonStyle);
    stepButton->setToolTip("单步调试");

    // 速度调整滑块
    speedSlider = new QSlider(Qt::Horizontal);
    speedSlider->setRange(-3, 3);
    speedSlider->setValue(0);
    speedSlider->setFixedWidth(120);
    speedSlider->setStyleSheet(R"(
        QSlider::groove:horizontal {
            height: 8px;
            background: #555;
            border-radius: 4px;
        }
        QSlider::handle:horizontal {
            background: #007ACC;
            border: 2px solid #005F9E;
            width: 18px;
            height: 18px;
            margin: -5px 0;
            border-radius: 9px;
        }
    )");

    speedSlider->installEventFilter(this); //安装事件过滤器，解决Qt中QSlider与ToolTip 的“显示延迟同步”问题

    // 当滑块值变化时，更新 ToolTip
    connect(speedSlider, &QSlider::valueChanged, [=](int value) {
        QString toolTipText = QString("当前速率: %1").arg(value);
        QToolTip::showText(QCursor::pos(), toolTipText, speedSlider);

        // 如果自动单步定时器正在运行，动态调整步速
        if (autoStepTimer->isActive()) {
            int interval = speedToInterval(value);
            autoStepTimer->start(interval);
        }
    });

    // 将按钮添加到控制面板布局
    controlLayout->addStretch();
    controlLayout->addWidget(buildButton);
    controlLayout->addStretch();
    controlLayout->addWidget(runPauseButton);
    controlLayout->addStretch();
    controlLayout->addWidget(stepButton);
    controlLayout->addStretch();
    controlLayout->addWidget(speedSlider);
    controlLayout->addStretch();

    // 右侧代码区域布局
    codeLayout->addWidget(controlPanel);
    codeLayout->addWidget(codeArea);
    codeLayout->setStretchFactor(codeArea, 3);
    codeLayout->setStretchFactor(controlPanel, 1);
}

void Widget::setupLayout()
{
    // 上方空白区域
    QWidget *topSpacer = new QWidget;
    topSpacer->setFixedHeight(20);

    // 主体布局
    QVBoxLayout *containerLayout = new QVBoxLayout(mainContainer);
    containerLayout->setContentsMargins(15, 15, 15, 15);
    containerLayout->setSpacing(10);
    containerLayout->addWidget(topSpacer);

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->setSpacing(10);
    mainLayout->addWidget(treeWidget);
    mainLayout->addWidget(verticalSplitter);
    mainLayout->addLayout(codeLayout);

    containerLayout->addLayout(mainLayout);

    QHBoxLayout *windowLayout = new QHBoxLayout(this);
    windowLayout->setContentsMargins(20, 20, 20, 20);
    windowLayout->addWidget(mainContainer);

    // 居中窗口
    QScreen *screen = QApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();  // 获取屏幕的几何信息
    int x = (screenGeometry.width() - this->width()) / 2;
    int y = (screenGeometry.height() - this->height()) / 2 - 30;
    this->move(x, y);

}

void Widget::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        dragPosition = event->globalPos() - frameGeometry().topLeft();
        event->accept();
    }
}

void Widget::mouseMoveEvent(QMouseEvent *event) {
    if (event->buttons() & Qt::LeftButton) {
        move(event->globalPos() - dragPosition);
        event->accept();
    }
}

void Widget::onTreeItemClicked(QTreeWidgetItem *item, int column)
{
    if (item->childCount() > 0) return; // 父目录不执行任何操作

    visualAreaView->resetTransform();
    visualAreaScene->clear();
    logArea->clear();

    if (gdbProcess->state() == QProcess::Running) {
        executionRunning = false;
        runPauseButton->setIcon(QIcon(":/icons/run.png"));
        runPauseButton->setToolTip("运行");
        gdbProcess->write("quit\n");
        autoStepTimer->stop();
    }

    QString itemText = item->text(column).trimmed();  // 获取被点击项的文本

    // 建立文件映射：导航项 -> .c 文件
    QMap<QString, QString> fileMap = {
        {"链表", ":/codes/singlylinkedlist.c"},
        {"栈", ":/codes/stack.c"},
        {"队列", ":/codes/queue.c"},
        {"先序遍历", ":/codes/prebinarytree.c"},
        {"中序遍历", ":/codes/inbinarytree.c"},
        {"后序遍历", ":/codes/postbinarytree.c"},
        {"平衡二叉树", ":/codes/avltree.c"},
        {"深度优先搜索", ":/codes/dfs.c"},
        {"宽度优先搜索", ":/codes/bfs.c"},
        {"Dijkstra", ":/codes/dijkstra.c"},
        {"Floyd", ":/codes/floyd.c"},
        {"折半查找", ":/codes/binarysearch.c"},
        {"B树", ":/codes/btree.c"},
        {"冒泡排序", ":/codes/bubblesort.c"},
        {"快速排序", ":/codes/quicksort.c"},
        {"堆排序", ":/codes/heapsort.c"},
        {"桶排序", ":/codes/bucketsort.c"}
    };

    // 检查是否存在对应的 .c 资源文件
    if (fileMap.contains(itemText)) {
        filePath = fileMap[itemText];

        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QMessageBox::warning(this, "错误", "无法加载文件：" + filePath);
            return;
        }

        // 指定读取的编码格式为 UTF-8，防止中文乱码
        QTextStream in(&file);
        in.setCodec("UTF-8");
        QString codeContent = in.readAll();
        file.close();

        // 加载到 codeArea
        codeArea->setPlainText(codeContent);
        highlighter = new CHighlighter(codeArea->document());
    } else {
        QMessageBox::warning(this, "提示", "没有找到对应的代码文件");
    }
}

void Widget::compileCode()
{
    // 获取代码内容
    QString code = codeArea->toPlainText();
    if (code.trimmed().isEmpty()) {
        logArea->appendPlainText(" 代码为空，无法编译！");
        return;
    }

    // 保存代码到临时文件 `temp_code.c`
    QString filePath = QDir::currentPath() + "/temp_code.c";
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        logArea->appendPlainText(" 无法保存代码文件！");
        return;
    }
    QTextStream out(&file);
    out << code;
    file.close();

    // 生成输出文件 `temp_code.exe`
    QString outputFile = "temp_code.exe";
    QString compileCommand = QString("gcc \"%1\" -o \"%2\" -g -Wall -Wextra -std=c99").arg(filePath, outputFile);

    // 创建编译进程
    compilerProcess = new QProcess(this);
    compilerProcess->setProcessChannelMode(QProcess::MergedChannels);

    connect(compilerProcess, &QProcess::readyRead, [=]() {
        logArea->appendPlainText(compilerProcess->readAll());
    });

    connect(compilerProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            [=](int exitCode, QProcess::ExitStatus status) {
                if (status == QProcess::NormalExit && exitCode == 0) {
                    logArea->appendPlainText(" 编译成功！");
                } else {
                    logArea->appendPlainText(" 编译失败，请检查错误！");
                }
                compilerProcess->deleteLater();
    });

    // 执行编译命令
    compilerProcess->start(compileCommand);
}

void Widget::startGDBProcess()
{
    // 初始化gdb进程
    gdbProcess = new QProcess(this);
    gdbProcess->setProcessChannelMode(QProcess::MergedChannels);

    // 启动定时器，实现自动运行功能
    autoStepTimer = new QTimer(this);
    connect(autoStepTimer, &QTimer::timeout, [=]() {
        if (stepButton->isEnabled()) {
            stepButton->click();  // 自动触发单步执行
        }
    });

    connect(gdbProcess, &QProcess::readyReadStandardOutput, [=]() {
        QString output = gdbProcess->readAllStandardOutput();
        //logArea->appendPlainText(output);

        if(filePath != NULL)
        {
            doVisualWork(output);
        }

        // 匹配“at路径”格式（代码行高亮）
        QRegularExpression regexWithPath(R"(at [^\n]*[\\/]temp_code\.c:(\d+))");
        QRegularExpressionMatch matchWithPath = regexWithPath.match(output);
        int lineNumber = -1;
        if (matchWithPath.hasMatch()) {
            lineNumber = matchWithPath.captured(1).toInt();
        } else {
            // 匹配“直接行号”格式
            QRegularExpression regexLineOnly(R"(^(\d+)\s+.+)");  // 行首是数字后面跟空格
            QRegularExpressionMatch matchLineOnly = regexLineOnly.match(output);
            if (matchLineOnly.hasMatch()) {
                lineNumber = matchLineOnly.captured(1).toInt();
            }
        }
        if (lineNumber != -1) {
            highlightLine(lineNumber); // 调用高亮函数
        }

        // 程序结束检测
        if (output.contains("exited normally") || output.contains("exited with code")) {
            logArea->appendPlainText(" 程序执行完毕。");
            executionRunning = false;
            runPauseButton->setIcon(QIcon(":/icons/run.png"));
            runPauseButton->setToolTip("运行");

            gdbProcess->write("quit\n");
            autoStepTimer->stop();  // 停止计时

            deleteVisualEntity();
        }
    });
}

void Widget::onRunPauseButtonClicked()
{
    if (executionRunning) {

        // 先检查是否存在 temp_code.exe
        if (!QFile::exists("temp_code.exe")) {
            logArea->appendPlainText(" 错误：未找到可执行文件 temp_code.exe！");
            executionRunning = false;
            runPauseButton->setIcon(QIcon(":/icons/run.png"));
            runPauseButton->setToolTip("运行");
            return;
        }

        // 启动 GDB 或继续执行
        if (gdbProcess->state() != QProcess::Running) {

            visualAreaView->resetTransform();
            visualAreaScene->clear();

            activeVisualEntity();

            gdbProcess->start("gdb temp_code.exe");
            if (!gdbProcess->waitForStarted(1000)) {
                logArea->appendPlainText("无法启动GDB！");
                executionRunning = false;
                runPauseButton->setIcon(QIcon(":/icons/run.png"));
                runPauseButton->setToolTip("运行");
                return;
            }
            gdbProcess->write("start\n");  // 从main处开始执行
        }
        // 启动定时器模拟连续单步执行
        int interval = speedToInterval(speedSlider->value());
        autoStepTimer->start(interval);
    } else {
        gdbProcess->write("interrupt\n");
        autoStepTimer->stop();  // 停止自动单步执行
    }
}

int Widget::speedToInterval(int sliderValue)
{
    switch (sliderValue) {
            case -3: return 3200;
            case -2: return 2400;
            case -1: return 1600;
            case 0:  return 800;
            case 1:  return 400;
            case 2:  return 200;
            case 3:  return 100;
            default: return 500;
        }
}

void Widget::highlightLine(int lineNum) {
    if (lineNum < 0) return;

    QTextDocument *doc = codeArea->document();
    QTextBlock block = doc->findBlockByNumber(lineNum - 1);
    if (!block.isValid()) return;

    QTextCursor cursor(block);
    cursor.select(QTextCursor::LineUnderCursor);

    // 清除之前的高亮
    codeArea->setExtraSelections(QList<QTextEdit::ExtraSelection>());

    // 设置当前行的高亮
    QList<QTextEdit::ExtraSelection> extraSelections;
    QTextEdit::ExtraSelection selection;
    QColor highlightColor(255, 255, 0, 50);
    selection.format.setBackground(highlightColor);
    selection.format.setProperty(QTextFormat::FullWidthSelection, true);
    selection.cursor = cursor;
    extraSelections.append(selection);

    codeArea->setExtraSelections(extraSelections);

    QTextCursor tempCursor(block);
    codeArea->setTextCursor(tempCursor);  // 临时定位
    codeArea->ensureCursorVisible();
}

bool Widget::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == speedSlider && event->type() == QEvent::ToolTip) {
        int value = speedSlider->value();
        QString tip = QString("当前速率: %1").arg(value);
        QToolTip::showText(QCursor::pos(), tip, speedSlider);
        return true; // 表示手动处理了 Tooltip
    }
    return QWidget::eventFilter(watched, event);
}

void Widget::doVisualWork(QString output)
{
    if(filePath == ":/codes/prebinarytree.c" || filePath == ":/codes/inbinarytree.c" || filePath == ":/codes/postbinarytree.c")
    {
        QRegularExpression insertRegex("INSERT (\\d+)");
        QRegularExpression visitRegex("VISIT (\\d+)");
        QRegularExpressionMatch match;
        if ((match = insertRegex.match(output)).hasMatch()) {
            logArea->appendPlainText(match.captured(0));

            int value = match.captured(1).toInt();
            bstTree->insert(value);
            visualTree->insertNodeDynamic(value, bstTree->getRoot());
        }
        else if ((match = visitRegex.match(output)).hasMatch()) {
            logArea->appendPlainText(match.captured(0));

            int value = match.captured(1).toInt();
            visualTree->highlightNode(value);
        }
    }
    else if(filePath == ":/codes/avltree.c")
    {
        QRegularExpression insertRegex(R"(INSERT (\d+))");
        QRegularExpression deleteRegex(R"(DELETE (\d+))");

        QRegularExpressionMatch match;

        if ((match = insertRegex.match(output)).hasMatch()) {
            logArea->appendPlainText(match.captured(0));

            int value = match.captured(1).toInt();
            avlTree->insert(value);
            visualTree->rebuild(avlTree->getRoot());
        }
        else if ((match = deleteRegex.match(output)).hasMatch()) {
            logArea->appendPlainText(match.captured(0));

            int value = match.captured(1).toInt();
            avlTree->remove(value);
            visualTree->rebuild(avlTree->getRoot());
        }
    }
    else if(filePath == ":/codes/singlylinkedlist.c")
    {
        QRegularExpression insertHeadRegex(R"(INSERT_HEAD (\d+))");
        QRegularExpression insertTailRegex(R"(INSERT_TAIL (\d+))");
        QRegularExpression deleteRegex(R"(DELETE (\d+))");
        QRegularExpression findRegex(R"(FIND (\d+))");
        QRegularExpression nextRegex(R"(NEXT (\d+))");

        QRegularExpressionMatch match;

        if ((match = insertHeadRegex.match(output)).hasMatch()) {
            int value = match.captured(1).toInt();
            logArea->appendPlainText(match.captured(0));
            singlylist->insertAtHead(value);
            visualsinglylist->insertAtHeadAnimated(value);
        }
        else if ((match = insertTailRegex.match(output)).hasMatch()) {
            int value = match.captured(1).toInt();
            logArea->appendPlainText(match.captured(0));
            singlylist->insertAtTail(value);
            visualsinglylist->insertAtTailAnimated(value);
        }
        else if ((match = deleteRegex.match(output)).hasMatch()) {
            int value = match.captured(1).toInt();
            logArea->appendPlainText(match.captured(0));

            singlylist->deleteValue(value);
            visualsinglylist->deleteValueAnimated(value);
        }
        else if ((match = findRegex.match(output)).hasMatch()) {
            int value = match.captured(1).toInt();
            logArea->appendPlainText(match.captured(0));
            if(!visualsinglylist->findAndhighlight(value)) logArea->appendPlainText(QString("CAN NOT FIND %1").arg(value));
        }
        else if ((match = nextRegex.match(output)).hasMatch()) {
            int value = match.captured(1).toInt();
            //logArea->appendPlainText(match.captured(0));
            if(!visualsinglylist->nextAndhighlight(value)) logArea->appendPlainText(QString("CAN NOT FIND %1").arg(value));
        }
    }
    else if(filePath == ":/codes/stack.c")
    {
        QRegularExpression pushRegex(R"(PUSH (\d+))");
        QRegularExpression popRegex(R"(POP (\d+))");
        QRegularExpression topRegex(R"(TOP (\d+))");


        QRegularExpressionMatch match;

        if ((match = pushRegex.match(output)).hasMatch()) {
            int value = match.captured(1).toInt();
            logArea->appendPlainText(match.captured(0));

            stack->push(value);
            visualstack->pushAnimated(value);
        }
        else if ((match = popRegex.match(output)).hasMatch()) {
            logArea->appendPlainText(match.captured(0));

            stack->pop();
            visualstack->popAnimated();
        }
        else if ((match = topRegex.match(output)).hasMatch()) {
            logArea->appendPlainText(match.captured(0));

            visualstack->highlightTop();
        }
    }
    else if (filePath == ":/codes/queue.c")
    {
        QRegularExpression enqueueRegex(R"(ENQUEUE (\d+))");
        QRegularExpression dequeueRegex(R"(DEQUEUE (\d+))");
        QRegularExpression frontRegex(R"(FRONT (\d+))");

        QRegularExpressionMatch match;

        if ((match = enqueueRegex.match(output)).hasMatch()) {
            int value = match.captured(1).toInt();
            logArea->appendPlainText(match.captured(0));

            queue->enqueue(value);
            visualqueue->enqueueAnimated(value);
        }
        else if ((match = dequeueRegex.match(output)).hasMatch()) {
            logArea->appendPlainText(match.captured(0));

            queue->dequeue();
            visualqueue->dequeueAnimated();
        }
        else if ((match = frontRegex.match(output)).hasMatch()) {
            logArea->appendPlainText(match.captured(0));

            visualqueue->highlightFront();
        }
    }
    else if (filePath == ":/codes/dfs.c" || filePath == ":/codes/bfs.c")
    {
        QRegularExpression initRegex(R"(INIT (\d+))");
        QRegularExpression addEdgeRegex(R"(ADD_EDGE (\d+) (\d+))");
        QRegularExpression visitRegex(R"(VISIT (\d+))");
        QRegularExpression traverseRegex(R"(TRAVERSE (\d+) (\d+))");

        QRegularExpressionMatch match;
        if ((match = initRegex.match(output)).hasMatch()) {
            int vertices = match.captured(1).toInt();
            graph = new Graph(vertices);
            visualgraph->autoLayoutNodes(vertices);
            logArea->appendPlainText(match.captured(0));
        } else if ((match = addEdgeRegex.match(output)).hasMatch()) {
            int src = match.captured(1).toInt();
            int dest = match.captured(2).toInt();
            logArea->appendPlainText(match.captured(0));
            graph->addEdge(src, dest);
            visualgraph->addEdge(src, dest);
        } else if ((match = visitRegex.match(output)).hasMatch()) {
            int vertex = match.captured(1).toInt();
            logArea->appendPlainText(match.captured(0));
            visualgraph->highlightNode(vertex);
        } else if ((match = traverseRegex.match(output)).hasMatch()) {
            int src = match.captured(1).toInt();
            int dest = match.captured(2).toInt();
            logArea->appendPlainText(match.captured(0));
            visualgraph->highlightEdge(src, dest);
        }
    }
    else if (filePath == ":/codes/dijkstra.c")
    {
        QRegularExpression initRegex(R"(INIT (\d+))");
        QRegularExpression addEdgeRegex(R"(ADD_EDGE (\d+) (\d+) (\d+))");
        QRegularExpression visitRegex(R"(VISIT (\d+))");
        QRegularExpression traverseRegex(R"(TRAVERSE (\d+) (\d+))");

        QRegularExpressionMatch match;
        if ((match = initRegex.match(output)).hasMatch()) {
            int vertex = match.captured(1).toInt();
            graph = new Graph(vertex);
            visualgraph->autoLayoutNodes(vertex);
            logArea->appendPlainText(match.captured(0));
        }
        else if ((match = addEdgeRegex.match(output)).hasMatch()) {
            int src = match.captured(1).toInt();
            int dest = match.captured(2).toInt();
            int weight = match.captured(3).toInt();
            graph->addEdge(src, dest, weight);
            visualgraph->addEdge(src, dest, weight);
            logArea->appendPlainText(match.captured(0));
        }
        else if ((match = visitRegex.match(output)).hasMatch()) {
            int vertex = match.captured(1).toInt();
            visualgraph->highlightNode(vertex);
            logArea->appendPlainText(match.captured(0));
        }
        else if ((match = traverseRegex.match(output)).hasMatch()) {
            int src = match.captured(1).toInt();
            int dest = match.captured(2).toInt();
            visualgraph->Dj_highlightEdge(src, dest);
            logArea->appendPlainText(match.captured(0));
        }
    }
    else if (filePath == ":/codes/floyd.c")
    {
        QRegularExpression initRegex(R"(INIT (\d+))");
       QRegularExpression addEdgeRegex(R"(ADD_EDGE (\d+) (\d+) (\d+))");
       QRegularExpression updateRegex(R"(UPDATE (\d+) (\d+) (\d+))");

       static QVector<QVector<int>> distanceMatrix;
       static int vertices = 0;

       QRegularExpressionMatch match;

       if ((match = initRegex.match(output)).hasMatch()) {
           vertices = match.captured(1).toInt();
           graph = new Graph(vertices);
           visualgraph->autoLayoutNodes_Fd(vertices);

           distanceMatrix.resize(vertices);
           for (int i = 0; i < vertices; ++i) {
               distanceMatrix[i].resize(vertices);
               distanceMatrix[i].fill(INF);
               distanceMatrix[i][i] = 0;
           }
           visualgraph->initDistanceMatrix(vertices, distanceMatrix);
           logArea->appendPlainText(match.captured(0));
       }
       else if ((match = addEdgeRegex.match(output)).hasMatch()) {
           int src = match.captured(1).toInt();
           int dest = match.captured(2).toInt();
           int weight = match.captured(3).toInt();

           graph->addEdge(src, dest, weight);
           visualgraph->addEdge(src, dest, weight);

           distanceMatrix[src][dest] = weight;
           distanceMatrix[dest][src] = weight;
           visualgraph->updateDistanceMatrix(src, dest, weight);
           logArea->appendPlainText(match.captured(0));
       }
       else if ((match = updateRegex.match(output)).hasMatch()) {
           int src = match.captured(1).toInt();
           int dest = match.captured(2).toInt();
           int newDist = match.captured(3).toInt();

           distanceMatrix[src][dest] = newDist;
           distanceMatrix[dest][src] = newDist;
           visualgraph->updateDistanceMatrix(src, dest, newDist);

           logArea->appendPlainText(
               QString("更新路径：从 %1 到 %2 新距离为 %3").arg(src).arg(dest).arg(newDist));
       }
    }
    else if (filePath == ":/codes/btree.c") {
        QRegularExpression insertRegex(R"(INSERT (\d+))");
        QRegularExpression deleteRegex(R"(DELETE (\d+))");
        QRegularExpression findRegex(R"(FIND (\d+))");

        QRegularExpressionMatch match;

        if ((match = insertRegex.match(output)).hasMatch()) {
            int value = match.captured(1).toInt();
            logArea->appendPlainText(match.captured(0));

            btree->insert(value);
            visualbtree->insertAnimated(value);
        }
        else if ((match = deleteRegex.match(output)).hasMatch()) {
            int value = match.captured(1).toInt();
            logArea->appendPlainText(match.captured(0));

            btree->remove(value);
            visualbtree->removeAnimated(value);
        }
        else if ((match = findRegex.match(output)).hasMatch()) {
            int value = match.captured(1).toInt();
            logArea->appendPlainText(match.captured(0));

            BTreeNode* result = btree->search(value);
            if (result) {
                visualbtree->highlightNode(value);
            } else {
                logArea->appendPlainText(QString("CAN NOT FIND %1").arg(value));
            }
        }
    }
    else if (filePath == ":/codes/binarysearch.c") {


        QRegularExpression insertRegex(R"(INSERT (\d+))");
        QRegularExpression rangeCheckRegex(R"(RANGE (\d+) (\d+) (\d+))");
        QRegularExpression foundRegex(R"(FOUND_INDEX (\d+))");
        QRegularExpression notFoundRegex(R"(NOT_FOUND (\d+))");

        QRegularExpressionMatch match;
        if ((match = insertRegex.match(output)).hasMatch()) {
            int value = match.captured(1).toInt();
            visualbinarysearch->insertValue(value);
            logArea->appendPlainText(match.captured(0));
        } else if ((match = rangeCheckRegex.match(output)).hasMatch()) {
            int l = match.captured(1).toInt();
            int r = match.captured(2).toInt();
            int mid = match.captured(3).toInt();
            visualbinarysearch->highlightCheck(l, r, mid);
            logArea->appendPlainText(match.captured(0));
        } else if ((match = foundRegex.match(output)).hasMatch()) {
            int index = match.captured(1).toInt();
            visualbinarysearch->highlightFound(index);
            logArea->appendPlainText(match.captured(0));
        } else if ((match = notFoundRegex.match(output)).hasMatch()) {
            logArea->appendPlainText(match.captured(0));
        }
    }
    else if (filePath == ":/codes/bubblesort.c") {
        QRegularExpression insertRegex(R"(INSERT (\d+))");
        QRegularExpression compareRegex(R"(COMPARE (\d+) (\d+))");
        QRegularExpression swapRegex(R"(SWAP (\d+) (\d+))");
        QRegularExpression markRegex(R"(MARK_SORTED (\d+))");

        QRegularExpressionMatch match;

        if ((match = insertRegex.match(output)).hasMatch()) {
            int value = match.captured(1).toInt();
            logArea->appendPlainText(match.captured(0));
            visualbubblesort->insertValue(value);
        }
        else if ((match = compareRegex.match(output)).hasMatch()) {
            int i = match.captured(1).toInt();
            int j = match.captured(2).toInt();
            logArea->appendPlainText(match.captured(0));
            visualbubblesort->compare(i, j);
        }
        else if ((match = swapRegex.match(output)).hasMatch()) {
            int i = match.captured(1).toInt();
            int j = match.captured(2).toInt();
            logArea->appendPlainText(match.captured(0));
            visualbubblesort->swap(i, j);
        }
        else if ((match = markRegex.match(output)).hasMatch()) {
            int index = match.captured(1).toInt();
            logArea->appendPlainText(match.captured(0));
            visualbubblesort->markSorted(index);
        }
    }
    else if (filePath == ":/codes/quicksort.c") {
        QRegularExpression insertRegex(R"(INSERT (\d+))");
        QRegularExpression pivotRegex(R"(PIVOT (\d+) (\d+))");
        QRegularExpression compareARegex(R"(COMPARE_A (\d+) (\d+))");
        QRegularExpression compareBRegex(R"(COMPARE_B (\d+) (\d+))");
        QRegularExpression swapRegex(R"(SWAP (\d+) (\d+))");
        QRegularExpression markSortedRegex(R"(MARK_SORTED (\d+))");

        QRegularExpressionMatch match;

        if ((match = insertRegex.match(output)).hasMatch()) {
            int value = match.captured(1).toInt();
            visualquicksort->insertValue(value);
            logArea->appendPlainText(match.captured(0));
        }
        else if ((match = pivotRegex.match(output)).hasMatch()) {
            int index = match.captured(1).toInt();
            visualquicksort->setPivot(index);
            logArea->appendPlainText(match.captured(0));
        }
        else if ((match = compareARegex.match(output)).hasMatch()) {
            int index = match.captured(1).toInt();
            int pivotVal = match.captured(2).toInt();
            visualquicksort->compareA(index, pivotVal);
            logArea->appendPlainText(match.captured(0));
        }
        else if ((match = compareBRegex.match(output)).hasMatch()) {
            int index = match.captured(1).toInt();
            int pivotVal = match.captured(2).toInt();
            visualquicksort->compareB(index, pivotVal);
            logArea->appendPlainText(match.captured(0));
        }
        else if ((match = swapRegex.match(output)).hasMatch()) {
            int i = match.captured(1).toInt();
            int j = match.captured(2).toInt();
            visualquicksort->swap(i, j);
            logArea->appendPlainText(match.captured(0));
        }
        else if ((match = markSortedRegex.match(output)).hasMatch()) {
            int index = match.captured(1).toInt();
            visualquicksort->markSorted(index);
            logArea->appendPlainText(match.captured(0));
        }
    }
    else if (filePath == ":/codes/heapsort.c") {
        QRegularExpression insertRegex(R"(INSERT (\d+))");
        QRegularExpression swapRegex(R"(SWAP (\d+) (\d+))");
        QRegularExpression extractRegex(R"(EXTRACT_MAX (\d+))");
        QRegularExpression finishRegex(R"(SORT_FINISHED)");

        QRegularExpressionMatch match;
        if ((match = insertRegex.match(output)).hasMatch()) {
            int value = match.captured(1).toInt();
            visualheap->insert(value);
            logArea->appendPlainText(match.captured(0));
        }
        else if ((match = swapRegex.match(output)).hasMatch()) {
            visualheap->swap(match.captured(1).toInt(), match.captured(2).toInt());
        }
        else if ((match = extractRegex.match(output)).hasMatch()) {
            int last_idx = match.captured(1).toInt();
            visualheap->extractMax(last_idx);
            logArea->appendPlainText(match.captured(0));
        }else if((match = finishRegex.match(output)).hasMatch())
        {
            QStringList values;
            for (auto* node : visualheap->m_nodes)
                values << QString::number(node->value);
            logArea->appendPlainText("Final Heap: [" + values.join(", ") + "]");
        }
    }
    else if (filePath == ":/codes/bucketsort.c") {
        QRegularExpression insertRegex(R"(INSERT (\d+))");
        QRegularExpression assignRegex(R"(BUCKET_ASSIGN (\d+) (\d+))");
        QRegularExpression bucketSwapRegex(R"(SWAP (\d+) (\d+) (\d+))");
        QRegularExpression finalRegex(R"(FINAL_OUTPUT (\d+))");

        QRegularExpressionMatch match;

        if ((match = insertRegex.match(output)).hasMatch()) {
            int value = match.captured(1).toInt();
            logArea->appendPlainText(match.captured(0));
            visualbucketsort->insertValue(value);
        }
        else if ((match = assignRegex.match(output)).hasMatch()) {
            int value = match.captured(1).toInt();
            int bucket = match.captured(2).toInt();
            logArea->appendPlainText(match.captured(0));
            visualbucketsort->assignToBucket(value, bucket);
        }
        else if ((match = finalRegex.match(output)).hasMatch()) {
            int value = match.captured(1).toInt();
            logArea->appendPlainText(match.captured(0));
            visualbucketsort->markFinal(value);
        }
        else if ((match = bucketSwapRegex.match(output)).hasMatch()) {
            int i = match.captured(1).toInt();
            int j = match.captured(2).toInt();
            int bucket = match.captured(3).toInt();
            logArea->appendPlainText(match.captured(0));
            visualbucketsort->swapInBucket(i, j, bucket);
        }
    }
}

void Widget::activeVisualEntity()
{
    if (filePath == ":/codes/prebinarytree.c" || filePath == ":/codes/inbinarytree.c" || filePath == ":/codes/postbinarytree.c") {
        bstTree = new BSTTree();
        visualTree = new VisualTree(visualAreaScene, visualAreaView, visualAreaView->viewport()->size());
    } else if (filePath == ":/codes/avltree.c") {
        avlTree = new AVLTree();
        visualTree = new VisualTree(visualAreaScene, visualAreaView, visualAreaView->viewport()->size());
    }
    else if(filePath == ":/codes/singlylinkedlist.c")
    {
        singlylist = new SinglyLinkedList();
        visualsinglylist = new VisualSinglyLinkedList(visualAreaScene, visualAreaView, visualAreaView->viewport()->size());
    }
    else if(filePath == ":/codes/stack.c")
    {
        stack = new Stack;
        visualstack = new VisualStack(visualAreaScene, visualAreaView->viewport()->size());
    }
    else if(filePath == ":/codes/queue.c")
    {
        queue = new Queue;
        visualqueue = new VisualQueue(visualAreaScene, visualAreaView->viewport()->size());
    }
    else if (filePath == ":/codes/dfs.c" || filePath == ":/codes/bfs.c" || filePath == ":/codes/dijkstra.c" || filePath == ":/codes/floyd.c")
    {
        visualgraph = new VisualGraph(visualAreaScene, visualAreaView->viewport()->size());
    }
    else if(filePath == ":/codes/btree.c")
    {
        btree = new BTree;
        visualbtree = new VisualBTree(visualAreaScene, visualAreaView->viewport()->size(), visualAreaView);
    }
    else if(filePath == ":/codes/binarysearch.c")
    {
        visualbinarysearch = new VisualBinarySearch(visualAreaScene, visualAreaView->viewport()->size());
    }
    else if(filePath == ":/codes/bubblesort.c")
    {
        visualbubblesort = new VisualBubbleSort(visualAreaScene, visualAreaView->viewport()->size());
    }
    else if(filePath == ":/codes/quicksort.c")
    {
        visualquicksort = new VisualQuickSort(visualAreaScene, visualAreaView->viewport()->size());
    }
    else if(filePath == ":/codes/heapsort.c")
    {
        visualheap = new VisualHeap(visualAreaScene, visualAreaView, visualAreaView->viewport()->size());
    }
    else if(filePath == ":/codes/bucketsort.c")
    {
        visualbucketsort = new VisualBucketSort(visualAreaScene, visualAreaView->viewport()->size());
    }
}

void Widget::deleteVisualEntity()
{
    if(filePath == ":/codes/prebinarytree.c" || filePath == ":/codes/inbinarytree.c" || filePath == ":/codes/postbinarytree.c")
    {
        bstTree->clear();
        visualTree->clear();
        delete bstTree;
        delete visualTree;
    }
    else if(filePath == ":/codes/avltree.c")
    {
        avlTree->clear();
        visualTree->clear();
        delete avlTree;
        delete visualTree;
    }
    else if(filePath == ":/codes/singlylinkedlist.c")
    {
        singlylist->clear();
        visualsinglylist->clear();
        delete singlylist;
        delete visualsinglylist;
    }
    else if(filePath == ":/codes/stack.c")
    {
        stack->clear();
        visualstack->clear();
        delete stack;
        delete visualstack;
    }
    else if(filePath == ":/codes/queue.c")
    {
        queue->clear();
        visualqueue->clear();
        delete queue;
        delete visualqueue;
    }
    else if (filePath == ":/codes/dfs.c" || filePath == ":/codes/bfs.c" || filePath == ":/codes/dijkstra.c" || filePath == ":/codes/floyd.c")
    {
        graph->clear();
        visualgraph->clear();
        delete graph;
        delete visualgraph;
    }
    else if(filePath == ":/codes/btree.c")
    {
    }
    else if(filePath == ":/codes/binarysearch.c")
    {
        visualbinarysearch->clear();
        delete visualbinarysearch;
    }
    else if(filePath == ":/codes/bubblesort.c")
    {
        visualbubblesort->clear();
        delete visualbubblesort;
    }
    else if(filePath == ":/codes/quicksort.c")
    {
        visualquicksort->clear();
        delete visualquicksort;
    }
    else if(filePath == ":/codes/heapsort.c")
    {
        visualheap->clear();
        delete visualheap;
    }
    else if (filePath == ":/codes/bucketsort.c")
    {
        visualbucketsort->clear();
        delete visualbucketsort;
    }
}

Widget::~Widget()
{
    if (gdbProcess) {
            gdbProcess->write("quit\n");
            gdbProcess->terminate();
            gdbProcess->waitForFinished(100);
        }

    //删除构建时产生的临时文件
    QFile::remove(QDir::currentPath() + "/temp_code.c");
    QFile::remove(QDir::currentPath() + "/temp_code.exe");

    delete ui;
}
