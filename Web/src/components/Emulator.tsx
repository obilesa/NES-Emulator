import { useEffect, useRef } from "react";

import Games from "../api/games.json";

declare const FS: {
   mkdir(path: string, mode?: number): void;
   mount(fsType: string, opts: any, mountpoint: string): any;
    syncfs(populate: boolean, callback: any): void;
}
declare const IDBFS: string;

interface EmulatorProps {}

const Emulator: React.FC<EmulatorProps> = () => {
  const scriptMounted = useRef(false);
  const canvasRef = useRef<HTMLCanvasElement>(null);

  function initEmscriptenModule() {
    // var statusElement = document.getElementById('status');
    // var progressElement = document.getElementById('progress');
    // var spinnerElement = document.getElementById('spinner');

    window.Module = {
      preRun: [],
      postRun: [],
      print: (function () {
        /*
        var element = document.getElementById('output');
        if (element) element.value = ''; // clear browser cache
        return function(text) {
          if (arguments.length > 1) text = Array.prototype.slice.call(arguments).join(' ');
          // These replacements are necessary if you render to raw HTML
          //text = text.replace(/&/g, "&amp;");
          //text = text.replace(/</g, "&lt;");
          //text = text.replace(/>/g, "&gt;");
          //text = text.replace('\n', '<br>', 'g');
          console.log(text);
          if (element) {
            element.value += text + "\n";
            element.scrollTop = element.scrollHeight; // focus on bottom
          }
        };*/
      })(),
      canvas: (function () {
        var canvas = document.getElementById("canvas");

        // As a default initial behavior, pop up an alert when webgl context is lost. To make your
        // application robust, you may want to override this behavior before shipping!
        // See http://www.khronos.org/registry/webgl/specs/latest/1.0/#5.15.2
        canvas!.addEventListener(
          "webglcontextlost",
          function (e) {
            alert("WebGL context lost. You will need to reload the page.");
            e.preventDefault();
          },
          false
        );

        return canvas;
      })(),
      setStatus: function (text: string) {
        /*
        if (!Module.setStatus.last) Module.setStatus.last = { time: Date.now(), text: '' };
        if (text === Module.setStatus.last.text) return;
        var m = text.match(/([^(]+)\((\d+(\.\d+)?)\/(\d+)\)/);
        var now = Date.now();
        if (m && now - Module.setStatus.last.time < 30) return; // if this is a progress update, skip it if too soon
        Module.setStatus.last.time = now;
        Module.setStatus.last.text = text;
        if (m) {
          text = m[1];
          progressElement.value = parseInt(m[2])*100;
          progressElement.max = parseInt(m[4])*100;
          progressElement.hidden = false;
          spinnerElement.hidden = false;
        } else {
          progressElement.value = null;
          progressElement.max = null;
          progressElement.hidden = true;
          if (!text) spinnerElement.style.display = 'none';
        }
        statusElement.innerHTML = text;

         */
      },
      totalDependencies: 0,
      monitorRunDependencies: function (left: number) {
        this.totalDependencies = Math.max(this.totalDependencies, left);
        window.Module.setStatus(
          left ? "Preparing... (" + (this.totalDependencies - left) + "/" + this.totalDependencies + ")" : "All downloads complete."
        );
      },
      ccall: function (ident: string, returnType: string, argTypes: string[], args: any[]) {},
    };
    window.Module.setStatus("Downloading...");
    window.onerror = function (event) {
      // TODO: do not warn on ok events like simulating an infinite loop or exitStatus
      window.Module.setStatus("Exception thrown, see JavaScript console");
      //spinnerElement.style.display = "none";
      window.Module.setStatus = function (text) {
        if (text) console.error("[post-exception status] " + text);
      };
    };

    fetchGames();
  }

  function fetchGames() {
    
}

  useEffect(() => {
    // avoids the bug caused by restricted mode in react where the script is mounted twice
    if (scriptMounted.current) return;

    initEmscriptenModule();

    scriptMounted.current = true;
    const script = document.createElement("script");
    script.async = true;
    script.src = "Emulator/NES_Emulator.js";
    document.body.appendChild(script);

    return () => {
      document.body.removeChild(script);
    };
  }, []);

  let screenWidth = Math.floor((window.innerHeight / 7) * 8);

  // Emulator window
  return (
    <div className="canvasFrame">
      <canvas
        className={`emscripten h-[100vh]`}
        style={{
          width: screenWidth,
        }}
        id="canvas"
        onContextMenu={(e) => {
          e.preventDefault();
        }}
        tabIndex={-1}
      ></canvas>
    </div>
  );
};

export default Emulator;
