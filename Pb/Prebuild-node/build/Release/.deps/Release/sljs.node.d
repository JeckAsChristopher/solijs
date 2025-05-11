cmd_Release/sljs.node := ln -f "Release/obj.target/sljs.node" "Release/sljs.node" 2>/dev/null || (rm -rf "Release/sljs.node" && cp -af "Release/obj.target/sljs.node" "Release/sljs.node")
