/**
 * Copyright (C) 2013 Jake Woods <jake.f.woods@gmail.com>, Trent Houliston <trent@houliston.me>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#include "NUClear.h"

// Anonymous namespace to keep everything file local
namespace {
    
    struct TestObject {
        int x;
    };
    
    class NetworkEmitter : public NUClear::Reactor {
    public:
        NetworkEmitter(std::unique_ptr<NUClear::Environment> environment) : Reactor(std::move(environment)) {
            on<Trigger<Every<500, std::chrono::milliseconds>>>([this](const time_t& tick) {
                emit<Scope::NETWORK>(std::unique_ptr<TestObject>(new TestObject{5}));
            });
        }
    };
    
    class TestReactor : public NUClear::Reactor {
    public:
        
        TestReactor(std::unique_ptr<NUClear::Environment> environment) : Reactor(std::move(environment)) {
            
            // Trigger on a networked event
            on<Trigger<Network<TestObject>>>([this](const Network<TestObject>& message) {
                REQUIRE(message.data->x == 5);
                powerPlant->shutdown();
            });
        }
    };
}

TEST_CASE("Testing the Networking system", "[api][network][hidden]") {
    
    NUClear::PowerPlant::Configuration config;
    config.threadCount = 1;
    NUClear::PowerPlant plant(config);
    
    plant.install<NetworkEmitter>();
    plant.install<TestReactor>();
    
    plant.start();
}
