import React, { useEffect, useState } from "react";
import "./App.css";
import Footer from "./components/Footer";
import Header from "./components/Header";
import CursorContextProvider from "./contexts/CursorContext";
import Cursor from "./components/Cursor";
import Body from "./components/Body";
import Emulator from "./components/Emulator";

export type KeyDownEvent = {
  key: string;
  timeStamp: number;
};

function App() {
  const [lastKeyPressed, setLastKeyPressed] = React.useState<KeyDownEvent>({
    key: "",
    timeStamp: 0,
  });
  const [MenuSelected, SetMenuSelected] = useState(0);
  function handleKeyDown(e: KeyboardEvent) {
    // TODO: Only update when a relevant key has been pressed
    setLastKeyPressed({ key: e.key, timeStamp: e.timeStamp });
    if (e.key === "ArrowDown") {
      SetMenuSelected((currentMenu) => {
        if (currentMenu < 1) return currentMenu + 1;
        return currentMenu;
      });
    } else if (e.key === "ArrowUp") {
      SetMenuSelected((currentMenu) => {
        if (currentMenu > 0) return currentMenu - 1;
        return currentMenu;
      });
    }
  }

  useEffect(() => {
    document.addEventListener("keydown", handleKeyDown);
    return () => document.removeEventListener("keydown", handleKeyDown);
  }, []);

  return (
    <div className="App bg-blue-700 min-h-screen flex flex-col">
      <CursorContextProvider>
        <Cursor height={0} width={0} x={0} y={0} active={MenuSelected} />
        <Header keyDown={lastKeyPressed} active={MenuSelected === 0} />
        <Body keyDown={lastKeyPressed} active={MenuSelected === 1} />
        <Footer />
      </CursorContextProvider>
      <Emulator />
    </div>
  );
}

export default App;
