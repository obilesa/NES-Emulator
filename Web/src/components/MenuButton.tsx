import React, { useEffect, useContext, useRef, useState } from "react";
import { CursorContext, Dimensions, Position } from "../contexts/CursorContext";
import MenuLabel from "./MenuLabel";
import { toHaveAccessibleName } from "@testing-library/jest-dom/matchers";

interface MenuButtonProps {
  icon: string;
  text: string;
  tabIndex: number;
  isSelected?: number;
  onMouseEnter: (child: number) => void;
  active: boolean;
}

const MenuButton: React.FC<MenuButtonProps> = ({ icon, text, tabIndex, isSelected, onMouseEnter, active }) => {
  const { setPosition, setDimensions } = useContext(CursorContext);
  const myRef = useRef<HTMLButtonElement>(null);

  const handleMouseEnter = () => {
    onMouseEnter(tabIndex);
  };

  useEffect(() => {
    if (isSelected === tabIndex && active) {
      const width = myRef.current?.getBoundingClientRect().width ?? 0;
      const height = myRef.current?.getBoundingClientRect().height ?? 0;
      const buttonDimensions = {
        width: width + window.innerWidth * 0.03,
        height: height + window.innerHeight * 0.018,
      } as Dimensions;

      const x = myRef.current?.getBoundingClientRect().x ?? 0;
      const y = myRef.current?.getBoundingClientRect().y ?? 0;

      const buttonPosition = { x: x - window.innerWidth * 0.015, y: y - window.innerHeight * 0.007 } as Position;

      setPosition(buttonPosition);

      setDimensions(buttonDimensions);
    }
  }, [isSelected, active]);

  return (
    <button
      ref={myRef}
      className={` 
      ${isSelected === tabIndex ? "scale-110" : "scale-100"}
       h-full w-[4.3vw] flex flex-row justify-center items-center  z-40  relative transition-all`}
      tabIndex={tabIndex}
      onMouseEnter={handleMouseEnter}
    >
      <img src={icon} alt="" className="z-40 relative h-full  object-contain pt-[1vh]   pixelated w-auto" />
      <MenuLabel label={text} hidden={!(isSelected === tabIndex && active)} />
    </button>
  );
};
export default MenuButton;
