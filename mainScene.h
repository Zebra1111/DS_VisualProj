#ifndef WIDGET_H
#define WIDGET_H

#include "chighlighter.h"
#include "graphicsview.h"
#include "bsttree.h"
#include "avltree.h"
#include "visualtree.h"
#include "singlylist.h"
#include "visualsinglylist.h"
#include "stack.h"
#include "visualstack.h"
#include "queue.h"
#include "visualqueue.h"
#include "graph.h"
#include "visualgraph.h"
#include "btree.h"
#include "visualbtree.h"
#include "visualsearch.h"
#include "visualbubblesort.h"
#include "visualquicksort.h"
#include "visualheap.h"
#include "visualbucketsort.h"
#include <QWidget>
#include <QMainWindow>
#include <QApplication>
#include <QDebug>
#include <QDockWidget>
#include <QTreeWidget>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QWidget>
#include <QSplitter>
#include <QLabel>
#include <QPlainTextEdit>
#include <QToolButton>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QGraphicsDropShadowEffect>
#include <QPushButton>
#include <QIcon>
#include <QSlider>
#include <QToolTip>
#include <QScreen>
#include <QFileDialog>
#include <QFileSystemWatcher>
#include <QFont>
#include <QTextStream>
#include <QMessageBox>
#include <QProcess>
#include <QTimer>
#include <QThread>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QVector>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE


class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

    void initWindow();                 // 初始化窗口属性
    void createMainContainer();        // 创建主容器
    void createCloseButton();          // 创建关闭按钮
    void createTreeWidget();           // 创建左侧导航栏
    void createVisualizationArea();    // 创建可视化与日志区域
    void createCodeAndButtonArea();    // 创建代码编辑与控制按钮区域
    void setupLayout();                // 布局管理

    void onTreeItemClicked(QTreeWidgetItem *item, int column); // 点击导航栏
    void compileCode();                                        // 代码构建
    void startGDBProcess();                                    // 配置和启动GDB进程
    void onRunPauseButtonClicked();                            // 执行代码
    void highlightLine(int lineNum);                           // 根据行号高亮 codeArea 中对应的行
    int speedToInterval(int sliderValue);                      // 运行速度调整

    void activeVisualEntity();               // 定义可视化对象
    void deleteVisualEntity();               // 释放可视化对象
    void doVisualWork(QString output);       // 执行可视化任务

    //解决Qt中QSlider与ToolTip 的“显示延迟同步”问题
    bool eventFilter(QObject *watched, QEvent *event) override;

    //实现窗口拖动
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;


private:
    Ui::Widget *ui;
    QPoint dragPosition; //实现窗口拖动

    // UI 组件
    QWidget *mainContainer;
    QTreeWidget *treeWidget;
    QGraphicsView *visualAreaView;
    QGraphicsScene *visualAreaScene;
    QSplitter *verticalSplitter;
    QPlainTextEdit *logArea;
    QVBoxLayout *codeLayout;
    QTextEdit *codeArea;
    QToolButton *buildButton;
    QToolButton *runPauseButton;
    QToolButton *stepButton;
    QSlider *speedSlider;

    bool executionRunning;     // 当前是否处于“运行”状态
    CHighlighter *highlighter; // 代码高亮
    QProcess *compilerProcess; // 代码编译
    QProcess *gdbProcess;      // 运行C语言代码（调试模式）
    QTimer *autoStepTimer;     // 自动单步执行定时器(自动运行)
    QString filePath;          // 唯一标识可视化程序

    // 二叉树可视化
    BSTTree *bstTree;
    AVLTree *avlTree;
    VisualTree *visualTree;

    // 链表可视化
    SinglyLinkedList *singlylist;
    VisualSinglyLinkedList *visualsinglylist;

    // 栈可视化
    Stack *stack;
    VisualStack *visualstack;

    // 队列可视化
    Queue *queue;
    VisualQueue *visualqueue;

    // 图可视化（DFS、BFS、Dijkstra、Floyd）
    Graph *graph;
    VisualGraph *visualgraph;

    // B树可视化
    BTree *btree;
    VisualBTree *visualbtree;

    // 折半查找可视化
    VisualBinarySearch *visualbinarysearch;

    // 冒泡排序可视化
    VisualBubbleSort *visualbubblesort;

    // 快速排序可视化
    VisualQuickSort *visualquicksort;

    // 堆排序可视化
    VisualHeap *visualheap;

    // 桶排序可视化
    VisualBucketSort *visualbucketsort;

};
#endif // WIDGET_H
