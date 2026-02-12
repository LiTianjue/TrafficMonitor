#include "crow.h"
#include "crow/middlewares/session.h"
#include "crow/middlewares/cookie_parser.h"
#include "src/datamock.hpp"
#include "src/realtimemock.hpp"
#include "src/easydb.hpp"
#include "include/mock.hpp"
#include <stdio.h>

using Session = crow::SessionMiddleware<crow::InMemoryStore>;

class APP
{
public:
	APP() {init();}
	void run() {
		app.port(18080).run();
	}
private:
    bool is_authenticated(const crow::request& req) {
        auto& session = app.get_context<Session>(req);
        return session.get("auth", false);
    }

	template <typename T>
	crow::response renderPOST(const crow::request &req)
	{
        if (!is_authenticated(req)) return crow::response(401);
		T t;
		struct_json::from_json(t,req.body);
		db.write(t);
		return std::move(crow::response(200,"OK"));
	}
	template <typename T>
	crow::response renderGET(const crow::request &req)
	{
        if (!is_authenticated(req)) return crow::response(401);
		T t;
		db.read(t);
		std::string val ;
		struct_json::to_json(t,val);

		return std::move(crow::response(200,val));
	}


	void init() 
	{
        // Login API
        CROW_ROUTE(app, "/api/login").methods("POST"_method)
        ([this](const crow::request& req) {
            auto x = crow::json::load(req.body);
            if (!x) return crow::response(400);
            
            if (x["username"].s() == "admin" && x["password"].s() == "admin") {
                auto& session = app.get_context<Session>(req);
                session.set("auth", true);
                return crow::response(200, "OK");
            }
            return crow::response(401, "Invalid credentials");
        });

        // Logout
        CROW_ROUTE(app, "/api/logout").methods("POST"_method)
        ([this](const crow::request& req) {
            auto& session = app.get_context<Session>(req);
            session.remove("auth");
            return crow::response(200, "Logged out");
        });

        // Check Auth Status
        CROW_ROUTE(app, "/api/me").methods("GET"_method)
        ([this](const crow::request& req) {
			if (!is_authenticated(req))  {
				return crow::response(401);
			}
			return std::move(crow::response(200,"{\"username\":\"admin\"}"));
        });

        // Static Files Serving
        CROW_ROUTE(app, "/")
        ([](const crow::request&, crow::response& res){
            res.set_static_file_info("web/dist/index.html");
            res.end();
        });
        
        CROW_ROUTE(app, "/assets/<path>")
        ([](const crow::request&, crow::response& res, std::string path){
            res.set_static_file_info("web/dist/assets/" + path);
            res.end();
        });

        CROW_ROUTE(app, "/favicon.ico")
        ([](const crow::request&, crow::response& res){
             res.set_static_file_info("web/dist/favicon.ico");
             res.end();
        });

		CROW_ROUTE(app, "/monitor").methods("POST"_method)  ([this](const crow::request &req) {
                if (!is_authenticated(req)) return crow::response(401);
				QueryParam p;
				struct_json::from_json(p,req.body);
				QueryReturnDatas data;
				data = mock::getQueryData(p);
				std::string ret;
				struct_json::to_json(data,ret);
		return std::move(crow::response(200,ret));
		});
	
		//[1]信号机配置
		CROW_ROUTE(app, "/TscConfig").methods("POST"_method)  ([this](const crow::request &req) {
			return renderPOST<TscConfig>(req);
		});
		CROW_ROUTE(app, "/TscConfig").methods("GET"_method)  ([this](const crow::request &req) {
			return renderGET<TscConfig>(req);
		});

		//[2] 检测器表
		CROW_ROUTE(app, "/DetectorTable").methods("POST"_method)  ([this](const crow::request &req) {
			return renderPOST<DetectorTable>(req);
		});
		CROW_ROUTE(app, "/DetectorTable").methods("GET"_method)  ([this](const crow::request &req) {
			return renderGET<DetectorTable>(req);
		});

		//[3] 通道配置
		CROW_ROUTE(app, "/ChannelCtrlTable").methods("POST"_method)  ([this](const crow::request &req) {
			return renderPOST<ChannelCtrlTable>(req);
		});
		CROW_ROUTE(app, "/ChannelCtrlTable").methods("GET"_method)  ([this](const crow::request &req) {
			return renderGET<ChannelCtrlTable>(req);
		});

		//[4] 手动关联
		CROW_ROUTE(app, "/ManualRelateInfoTable").methods("POST"_method)  ([this](const crow::request &req) {
			return renderPOST<ManualRelateInfoTable>(req);
		});
		CROW_ROUTE(app, "/ManualRelateInfoTable").methods("GET"_method)  ([this](const crow::request &req) {
			return renderGET<ManualRelateInfoTable>(req);
		});

		//[5] 属性配置
		CROW_ROUTE(app, "/TrafficAttrbuteTable").methods("POST"_method)  ([this](const crow::request &req) {
			return renderPOST<TrafficAttrbuteTable>(req);
		});
		CROW_ROUTE(app, "/TrafficAttrbuteTable").methods("GET"_method)  ([this](const crow::request &req) {
			return renderGET<TrafficAttrbuteTable>(req);
		});

		//[6] 方案配置
		CROW_ROUTE(app, "/PssSchemeTable").methods("POST"_method)  ([this](const crow::request &req) {
			return renderPOST<PssSchemeTable>(req);
		});
		CROW_ROUTE(app, "/PssSchemeTable").methods("GET"_method)  ([this](const crow::request &req) {
			return renderGET<PssSchemeTable>(req);
		});

		//[7]调度配置
		CROW_ROUTE(app, "/PssTrigerTable").methods("POST"_method)  ([this](const crow::request &req) {
			return renderPOST<PssTrigerTable>(req);
		});
		CROW_ROUTE(app, "/PssTrigerTable").methods("GET"_method)  ([this](const crow::request &req) {
			return renderGET<PssTrigerTable>(req);
		});


		//实时状态测试
		CROW_ROUTE(app, "/PssRealTimeStatus").methods("GET"_method)  ([this](const crow::request &req) {
                if (!is_authenticated(req)) return crow::response(401);
				printf("rt \n");
				PssRealTimeStatus rt;
				mock::getRealtime(rt);
				std::string js;
				struct_json::to_json(rt,js);
				return std::move(crow::response(200,js));
		});

		// DetectorStatusTable GET 方法
		CROW_ROUTE(app, "/DetectorStatusTable").methods("GET"_method)  ([this](const crow::request &req) {
                if (!is_authenticated(req)) return crow::response(401);
				std::string data = mock::load<std::vector<DetectorDeviceStatus>>();
				return std::move(crow::response(200,data));
		});

		// TscOnlineStatus GET 方法
		CROW_ROUTE(app, "/TscOnlineStatus").methods("GET"_method)  ([this](const crow::request &req) {
                if (!is_authenticated(req)) return crow::response(401);
				std::string data = mock::load<TscOnlineStatus>();
				return std::move(crow::response(200,data));
		});

		// TSC/DetectorTable GET 方法
		CROW_ROUTE(app, "/TSC/DetectorTable").methods("GET"_method)  ([this](const crow::request &req) {
			return renderGET<DetectorTable>(req);
		});

		// TSC/ChannelCtrlTable GET 方法
		CROW_ROUTE(app, "/TSC/ChannelCtrlTable").methods("GET"_method)  ([this](const crow::request &req) {
			return renderGET<ChannelCtrlTable>(req);
		});

		// MonitorKanban GET 方法 - 获取事件日志
		CROW_ROUTE(app, "/MonitorKanban").methods("GET"_method)  ([this](const crow::request &req) {
                if (!is_authenticated(req)) return crow::response(401);
				std::string data = mock::load<std::vector<EventLog>>();
				return std::move(crow::response(200,data));
		});

		// fetchTagValue POST 方法 - 根据tag获取对应value
		CROW_ROUTE(app, "/fetchTagValue").methods("POST"_method)  ([this](const crow::request &req) {
                if (!is_authenticated(req)) return crow::response(401);
				
				// 解析请求体获取tag
				auto json_data = crow::json::load(req.body);
				if (!json_data || !json_data.has("tag")) {
					return crow::response(400, "Missing tag parameter");
				}
				
				std::string tag = json_data["tag"].s();
				
				// 根据tag生成对应的随机整数值
				int value = 0;
				if (tag == "total_flow") {
					value = rand() % 200 + 50; // 50-250
				} else if (tag == "queue_length") {
					value = rand() % 150 + 10; // 10-160
				} else if (tag == "vehicle_count") {
					value = rand() % 100 + 20; // 20-120
				} else if (tag == "signal_duration") {
					value = rand() % 120 + 30; // 30-150
				} else {
					value = rand() % 1000; // 默认 0-999
				}
				
				// 返回JSON格式的响应
				crow::json::wvalue response;
				response["tag"] = tag;
				response["value"] = value;
				
				return std::move(crow::response(200, response));
		});
	}
	
private:
    crow::App<crow::CookieParser, Session> app{Session{
        crow::CookieParser::Cookie("session").max_age(24 * 60 * 60).path("/"),
        16, // session id length
        crow::InMemoryStore{}
    }};
	EasyDB db;

};
