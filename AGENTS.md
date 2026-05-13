# 项目说明

## 项目概述
交通信号控制监控系统，C++后端（使用Crow框架）+ Vue 3前端。

## 构建命令

### 后端（C++）
```bash
git submodule update --init --recursive  # 首次需要初始化子模块
cmake -B build && cmake --build build
./build/myapp
```
- 服务运行在 18080 端口

### 前端（Vue 3）
```bash
cd web
npm install
npm run dev     # 开发服务器，自动代理API到18080
npm run build   # 构建产物输出到 web/dist/
```
- 需要先构建前端再运行后端（后端从 web/dist/ serve 静态文件）

## 项目结构
- **后端入口**: `main.cpp` → `app.hpp`（Crow路由定义于此）
- **前端入口**: `web/src/main.js` + `web/src/App.vue`
- **API代理**: `web/vite.config.js` 配置

## 关键文件
- `app.hpp`: 所有REST API端点（登录、TscConfig、DetectorTable、ChannelCtrlTable、TrafficAttributeTable等）
- `vite.config.js`: API代理配置
- `CMakeLists.txt`: C++构建配置（C++17）

## 开发账号
- 用户名: `admin`，密码: `admin`

## 第三方依赖
- `third/Crow`: Web框架
- `third/yalantinglibs`: JSON序列化（struct_json）