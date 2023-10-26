import React, { useEffect } from "react";
import gearIcon from "../icons/gear.png";
import guideIcon from "../icons/guide.png";
import screenIcon from "../icons/screen.png";
import MenuButton from "./MenuButton";
import { KeyDownEvent } from "../App";

interface HeaderProps {
  keyDown: KeyDownEvent;
  active: boolean;
}

const grid_lines = Array.from({ length: 10 }, (_, i) => (
  <div className="w-[0.5%] bg-black h-[60%] " key={i}>
    {" "}
  </div>
));

const horizontal_lines = Array.from({ length: 4 }, (_, i) => (
  <div className=" h-[0.8vh] bg-gray-800 z-20 mb-[0.4vh] w-[8vw]" key={i}>
    {" "}
  </div>
));

const Header: React.FC<HeaderProps> = ({ keyDown, active }) => {
  const [selectedChild, setSelectedChild] = React.useState(0);
  const MenuWrapper = React.useRef<HTMLDivElement>(null);
  const handleMouseEnter = (child: number) => {
    setSelectedChild(child);
  };

  useEffect(() => {
    if (!active) return;
    if (keyDown.key === "ArrowLeft") {
      setSelectedChild(() => {
        if (selectedChild - 1 < 0) {
          if (MenuWrapper.current) {
            return MenuWrapper.current.children.length - 1;
          } else {
            return 0;
          }
        } else {
          return selectedChild - 1;
        }
      });
    } else if (keyDown.key === "ArrowRight") {
      setSelectedChild(() => {
        if (MenuWrapper.current) {
          return (selectedChild + 1) % MenuWrapper.current.children.length;
        }
        return 0;
      });
    }
  }, [keyDown.timeStamp]);

  return (
    <header className="relative">
      <div className="h-[4vh] bg-gray-800 w-full relative">
        <div className="ml-[7vw] h-full flex flex-row gap-[0.5%]">{grid_lines}</div>
      </div>
      <div className="bg-gray-400 h-[9vh] flex-row flex justify-center">
        <div ref={MenuWrapper} className="bg-gray-600 flex flex-row justify-center gap-[3vw] pl-[1.5vw] pr-[1.5vw]">
          <MenuButton tabIndex={0} isSelected={selectedChild} onMouseEnter={handleMouseEnter} icon={screenIcon} text="Screen" active={active} />
          <MenuButton tabIndex={1} isSelected={selectedChild} onMouseEnter={handleMouseEnter} icon={gearIcon} text="Settings" active={active} />
          <MenuButton tabIndex={2} isSelected={selectedChild} onMouseEnter={handleMouseEnter} icon={guideIcon} text="Guide" active={active} />
        </div>
      </div>

      <div className="bg-black w-[9vw] h-[103%] absolute right-[6vw] z-10 top-0  flex-col mt-[-0.4vh] flex items-center">{horizontal_lines}</div>
      <div className="bg-gray-800 h-[0.4vh] w-full"></div>
    </header>
  );
};

export default Header;
