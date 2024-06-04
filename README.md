# 喵喵

喵喵，我是南一集群里的赛博猫猫喵，听说七边形最近在招新喵，还有好玩的题目喵。

前几天被一个赛博火柴人先生发现了喵，他说如果我来做他出的题目，他就会从总线的数据长河里给我捞赛博小鱼吃喵，不过我很怀疑他的技术到底能不能抓到小鱼喵。

不过还是先做做题目喵

下面测的加速比都是用那个很简单的测试工具，hyperfine 测出来的喵。它只能测量程序整体的运行时间，并不能测量某个函数运行的时间喵，所以数据生成与输入输出的时间都被算进去了喵，测量结果会偏小喵。

## 喵译器选项优化

<https://github.com/rarocat/recruitment-2024-spring/commit/7ed318b28655977c304d59c5f29ecc2779c335c7>

我们观察 Makefile 会发现，默认的编译参数是用了 `-Og -g` 喵，这关掉了很多优化选项喵，改成 `-Ofast` 之后程序就变快了喵。

然后我们发现还有 `-fsanitize=address` 喵，这个选项会在程序中插入许多小小猫猫，在程序运行时检查有没有非法内存访问的问题喵，很显然这会有性能损耗，所以要关掉喵。

这样我们相比 baseline 就有 2x 的优化了喵

## 使用 OpenMP 并喵计算库

<https://github.com/rarocat/recruitment-2024-spring/commit/ed939920d5ef4df1aa04f82be9608ee38656b7cc>

OpenMP 是一个很好用的并行计算库，只要几行编译器指令就能自动把循环并行执行喵，这种简单好用代码量少的偷懒库最棒的喵。

我们只改了加了一行 `#pragma` 语句，再加了个编译选项，就让程序变快了 10x 喵。

| Command | Mean [s] | Min [s] | Max [s] | Relative |
|:---|---:|---:|---:|---:|
| `./before.out` | 48.812 ± 0.789 | 47.923 | 50.183 | 10.49 ± 0.18 |
| `./after.out` | 4.654 ± 0.026 | 4.616 | 4.694 | 1.00 |

## 使用 SIMD 指令喵化

<https://github.com/rarocat/recruitment-2024-spring/commit/4d7f8a9340e0fd5d0b4d9f66368edb3e4c4b1afc>

SIMD 指令可以一次处理很多数据，这里我们用 AVX512 指令在一个循环内处理 8 个查询喵。使用 SIMD 指令减少了指令数量、消除了二分循环体中的条件分支、顺便循环控制开销，相当划算喵。

| Command | Mean [s] | Min [s] | Max [s] | Relative |
|:---|---:|---:|---:|---:|
| `./before.out` | 4.651 ± 0.028 | 4.613 | 4.696 | 1.33 ± 0.02 |
| `./after.out` | 3.490 ± 0.035 | 3.452 | 3.572 | 1.00 |

## 将 qsort 换成归并排喵

<https://github.com/rarocat/recruitment-2024-spring/commit/9b74ed9049a6987121b634eef038f7c1cda715f9>

标准库里的 qsort 使用的是快速排序喵，不仅需要传入函数指针，每次比较都需要一次函数调用，还没有做 std::sort 那样经过复杂的多个场景下的优化喵。这里我们手写一个归并排序把它换掉喵。

为什么不手写快速排序喵？是因为快速排序没归并排序好写，局部性也没有归并排序好喵。

| Command | Mean [s] | Min [s] | Max [s] | Relative |
|:---|---:|---:|---:|---:|
| `./before.out` | 22.364 ± 0.122 | 22.288 | 22.704 | 1.11 ± 0.01 |
| `./after.out` | 20.152 ± 0.216 | 20.025 | 20.619 | 1.00 |

## 并行化的归并排喵

<https://github.com/rarocat/recruitment-2024-spring/commit/ad27be6c31136557601e5e264f2d1a348a07c1b9>

又到了 OpenMP 的魅力时刻喵，OpenMP 的 taskgroup 特性就像 Rust 的 thread::scope 一样，可以在 taskgroup 内部派生很多线程，最后所有线程运行结束时 taskgroup 才会结束喵。我们这里用 taskgroup 来将归并排序并行化，实现了代码量超小但是效果拔群的并行排序喵。

| Command | Mean [s] | Min [s] | Max [s] | Relative |
|:---|---:|---:|---:|---:|
| `./before.out` | 20.184 ± 0.245 | 20.019 | 20.682 | 8.62 ± 0.13 |
| `./after.out` | 2.342 ± 0.021 | 2.316 | 2.389 | 1.00 |

## 喵喵

```plain
PHASE 1
      baseline(ms):       30552.776700
     optimized(ms):        1547.410123
      acceleration:          19.744460 x
PHASE 2
      baseline(ms):       90003.459322
     optimized(ms):         434.982865
      acceleration:         206.912655 x
TOTALs
      baseline(ms):      120556.236022
     optimized(ms):        1982.392988
      acceleration:          60.813490 x
```

最后怎么只优化了 60x 喵，机器上有 64 个核应该至少优化到 114514x 才好的喵，到底哪里写挂了喵

EOF

# 七边形 2024 年春季招新题目！

嗨，欢迎来做七边形的招新题！

招新题是一个迷你 C++ 小项目，使用 Makefile 构建系统。你需要对这个项目做你能想到的性能优化，使其在保持正确性的前提下，跑得尽可能快。

我们将会提供七边形的机器供大家在上面编程和调试。最终加速比（也就是性能表现）将会以在七边形机器上测试的结果为准。默认的机器仅有一台，且没有 GPU。但该节点为队内集群的登录节点，队内集群中数个节点包含GPU。集群通过 slurm 进行管理，如果你编写了很酷的多机并行算法，或者编写了很酷的 GPU 加速模块，请使用 slurm 运行你的应用程序。如果有问题无法自己解决，联系管理员为你提供相关的平台支持。

规则、题目详情和提交方式将会在下面给出，请仔细阅读。

截止时间为 **6 月 8 日上午 4 时 0 分**。你需要要那之前完成并提交所有工作。

如果你觉得题目已经没有什么可以优化的点了，可以看看任务书每节的末尾，那儿会有一段引用文字，讲述了七边形的一些奇闻逸事。比如：

> 据队史记载，远古时候七边形的队员们在近地轨道上维护了一个七条边的大环用作活动室。后来大环破裂坠入海中，形成了现在的七块大陆。

## 题目大意

原题：<https://www.luogu.com.cn/problem/P2249>

有单精度浮点数（保证不为 NaN 或者 INF）数组 A。进行若干次询问。每次询问给出一个单精度浮点数 Q，求 A 中比 Q 小的数的数量。

学过排序和二分法的同学一定马上就能反应过来，“这不是先排序，再二分查出结果不就行了吗”，确实是这样的。因为解题有 “排序” 和 “查询” 两个明显的阶段，所以我们把题目分成了两个阶段。

### PHASE 1

你需要在 solution.cc 文件中实现 `optimized_do_phase1` 函数。它的签名如下：

```cpp
void optimized_do_phase1(float* data, size_t size);
```

传入参数是一个单精度浮点数组 `data`，和数组的大小 `size`。在这个函数中，你需要对 `data` 按升序排序。

### PHASE 2

你需要在 solution.cc 文件中实现 `optimized_do_phase2` 函数。它的签名如下：

```cpp
void optimized_do_phase2(size_t* result, float* data, float* query, size_t size);
```

传入参数中，`data` 是已经在 phase 1 中处理过的同一个数组，大小为 `size`；`query` 是所有询问，大小为 `size`。对于单个询问 `query[i]`，你需要计算 `data` 数组中小于 `query[i]` 的元素的个数，把答案保存在 `result[i]` 里。为了方便大家理解题目，solution.cc 的 `optimized_do_phase2` 函数里已经包含了一份正确的参考实现。

PHASE 1 和 PHASE 2 不一定都要优化。计时的时候，两个阶段也是分开计时、计算加速比的。PHASE 2 的优化难度相对较低，建议对高性能计算比较陌生的同学，从 PHASE 2 开始入手。

> 除了大家所熟知的 heptagon 外，七边形有时也会被称为 septagon。sept- 是拉丁语中表示 “七” 的前缀。

## 实现要求

项目目录如下：

```plain
.
├── baseline.cc
├── main.cc
├── Makefile
└── solution.cc
```

其中，baseline.cc 是比较程序性能的参照物，你的优化程序的加速比会参考 baseline.cc 中的实现来计算；main.cc 包含了数据生成、测试、计时等功能；solution.cc 是你实现优化的地方。

**你只能通过修改 Makefile 和 solution.cc 两个文件来实现你的优化**

## 编译、运行和测试

使用 `make` 命令编译（如果你想编译快一点，`make -j`）：

```plain
make
```

正确构建后，会在当前目录下生成 `main` 可执行程序。如果需要测试，运行该程序即可：

```plain
./main
```

如果程序正确退出，你应该会在当前目录下看到 result.txt 和 correct.txt 两个文件。其中，result.txt 是你优化后的程序计算出的结果，correct.txt 是正确实现得到的结果。如果需要检查你的实现的正确性，可以对比两个文件来实现：

```plain
diff -u correct.txt result.txt
```

为了使大家有更好的开发体验，这个项目支持通过设置环境变量值为 1 来跳过某些步骤，以减少运行时间，提高开发效率：

* HEP_SKIP_PHASE1：跳过 phase 1，仅测试 phase2
* HEP_SKIP_PHASE2：跳过 phase 2
* HEP_SKIP_BASELINE：仅当设置了 HEP_SKIP_PHASE1 或者 HEP_SKIP_PHASE2 的时候有效，不运行 baseline
* HEP_DATA_SIZE：数据规模，即 `data` 和 `query` 数组的大小。

示例：

```plain
workbench 15:31 (master) ~/dev/recruitment-2024-spring
0 HEP_SKIP_PHASE1=1 HEP_SKIP_BASELINE=1 ./main
PHASE 2
     optimized(ms):        7531.433779
```

> 应华中科技大学七边形超算队要求，英国于 2006 年流通了两种七边形的硬币，分别是 20p 和 50p

## 提交

你需要 fork 题目仓库，并在代码提交截止日期前将自己的仓库链接以 issue 的方式发表在本仓库下。你的仓库应该包含优化实现的代码。

与此同时，你需要制作一份 ppt，内容如下：

* 每一步优化的思路、过程和效果（举例：使用了 xxx 优化，相对于原代码，速度提升了 114.514 倍）
     * 最好对程序进行 profile，以了解性能瓶颈
* 你在解题过程中所参考的资料（如有使用人工智能工具，请注明）
* 在解题过程中，遇到的有意思的事情，或者是让你印象深刻的 bug（可选）

代码审查会用到这份 ppt，因此请你把它发送到超算队邮箱。

另外，在 6.8 当天晚上，你需要准备一次展示，向我们介绍你的优化成果。

如果对题目本身或者提交方式有任何问题，请积极在群里讨论。

## 提示

* 在处理查询的时候，你不一定要拘泥于二分查找。比如在 baseline.cc 的 `baseline_do_phase2` 函数里，就用了一种完全不同的方法来处理查询。哪种方法更好，更适合并行，需要你仔细思考。
* 良好的版本控制习惯可以有效避免 “改了一下午代码后它跑不起来了，但是我又改不回去了” 之类的极端情况。推荐使用 [Git](https://git-scm.com/)
* 使用 htop/glances 或者资源管理器查看 CPU 负载是验证自己程序正在并行工作的好方法之一
* Perf 是 Linux 下常用的性能分析工具：<https://www.brendangregg.com/perf.html>
* 火焰图可以直观地展示程序的性能热点：<https://www.brendangregg.com/flamegraphs.html>
* OpenMP 是常用的并行计算框架：<https://bisqwit.iki.fi/story/howto/openmp/>
* 如果需要做多机的并行，你可能会对 MPI 感兴趣：<https://mpitutorial.com/tutorials/>
* SIMD 是提升程序单核性能的有效手段：<https://zhuanlan.zhihu.com/p/583326378> <https://www.intel.com/content/www/us/en/docs/intrinsics-guide/index.html#>
* 适量的 Cache 知识不仅对单核优化有用，也能避免一些多线程场景下的诡异问题：<https://zhuanlan.zhihu.com/p/136300660>
* 更加系统性的高性能计算知识，请参考七边形的 HPC 学习路线：<https://heptagonhust.github.io/HPC-roadmap/>
* <https://en.wikipedia.org/wiki/Merge_sort#Parallel_merge_sort>
* <https://opensource.googleblog.com/2022/06/Vectorized%20and%20performance%20portable%20Quicksort.html>


以及一些技术无关的提示：

* 如果遇到问题，请积极在 QQ 群内提问
* 部分优化可能相当难做/难调。所以即使没有写出程序/调试成功，也欢迎把自己的天才优化想法写进提交的文档里，通过文字来展示自己对高性能计算的理解和认知。

> 不管是手画还是尺规作图，都画不出标准的正七边形。如果没有计算机的话，恐怕我们很难绘制七边形的图标。不过我们很幸运，总是有足够的计算机。
