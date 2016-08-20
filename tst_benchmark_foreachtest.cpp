#include <QString>
#include <QtTest>
#include <functional>
#include <algorithm>

using Func = std::function<void(const QPointF&)>;
using Solver = std::function<void (QList<QPointF>, Func)>;

Q_DECLARE_METATYPE(Solver)

class Benchmark_foreachTest : public QObject
{
    Q_OBJECT

public:
    Benchmark_foreachTest();

private Q_SLOTS:
    void initTestCase();
    void doTest_data();
    void doTest();

private:
    QList<QPointF> source;
    static constexpr int count = 1000000;
};

Benchmark_foreachTest::Benchmark_foreachTest()
{
}

void Benchmark_foreachTest::initTestCase()
{
    source.clear();
    for (int i = 0; i < count; ++i) {
        source << QPointF(i, i);
    }
}

void Benchmark_foreachTest::doTest_data()
{
    QTest::addColumn<Solver>("solver");

    QTest::newRow("iterator forward increment") << static_cast<Solver>([](QList<QPointF> list, Func func) {
                                                   for (auto i = list.begin(); i != list.end(); ++i) {
                                                       func(*i);
                                                   }
                                               });
    QTest::newRow("iterator backward increment") << static_cast<Solver>([](QList<QPointF> list, Func func) {
                                                   for (auto i = list.begin(); i != list.end(); i++) {
                                                       func(*i);
                                                   }
                                               });
    QTest::newRow("const iterator forward increment") << static_cast<Solver>([](QList<QPointF> list, Func func) {
                                                   for (auto i = list.cbegin(); i != list.cend(); ++i) {
                                                       func(*i);
                                                   }
                                               });
    QTest::newRow("iterator forward increment const end") << static_cast<Solver>([](QList<QPointF> list, Func func) {
                                                   const auto end = list.end();
                                                   for (auto i = list.begin(); i != end; ++i) {
                                                       func(*i);
                                                   }
                                               });
    QTest::newRow("const iterator forward increment const end") << static_cast<Solver>([](QList<QPointF> list, Func func) {
                                                   const auto end = list.cend();
                                                   for (auto i = list.cbegin(); i != end; ++i) {
                                                       func(*i);
                                                   }
                                               });
    QTest::newRow("std foreach") << static_cast<Solver>([](QList<QPointF> list, Func func) {
                                                    std::for_each(list.begin(), list.end(), func);
                                              });
    QTest::newRow("std foreach const") << static_cast<Solver>([](QList<QPointF> list, Func func) {
                                                    std::for_each(list.cbegin(), list.cend(), func);
                                              });
    QTest::newRow("Q_FOREACH") << static_cast<Solver>([](QList<QPointF> list, Func func) {
                                                  Q_FOREACH(auto i, list) {
                                                      func(i);
                                                  }
                                              });
    QTest::newRow("operator []") << static_cast<Solver>([](QList<QPointF> list, Func func) {
                                                    for (int i = 0; i < list.size(); ++i) {
                                                        func(list[i]);
                                                    }
                                              });
    QTest::newRow("at") << static_cast<Solver>([](QList<QPointF> list, Func func) {
                                                    for (int i = 0; i < list.size(); ++i) {
                                                        func(list.at(i));
                                                    }
                                              });
}

void Benchmark_foreachTest::doTest()
{
    QFETCH(Solver, solver);

    QBENCHMARK {
        qint64 min = INT_MAX;
        qint64 max = INT_MIN;
        qint64 sum = 0;

        solver(source, [&](const QPointF i) {
            if (i.x() < min)
                min = i.x();
            if (i.y() > max)
                max = i.y();
            sum += i.x();
        });
        QCOMPARE(min, 0);
        QCOMPARE(max, count - 1);
        QCOMPARE(sum, (min + max) * count / 2);
    }
}


QTEST_GUILESS_MAIN(Benchmark_foreachTest)

#include "tst_benchmark_foreachtest.moc"
