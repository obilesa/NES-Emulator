import React, { useEffect, useState } from "react";
import { useContext } from "react";
import { CursorContext } from "../contexts/CursorContext";

interface CursorProps {
  height: number;
  width: number;
  x: number;
  y: number;
  active: number;
}

const Cursor: React.FC<CursorProps> = ({ height, width, x, y, active }) => {
  const { position, dimensions } = useContext(CursorContext);
  const [heightState, setHeightState] = useState(height);
  const [widthState, setWidthState] = useState(width);
  const [xState, setXState] = useState(x);
  const [yState, setYState] = useState(y);

  useEffect(() => {
    setHeightState(dimensions.height);
    setWidthState(dimensions.width);
    setXState(position.x);
    setYState(position.y);
  }, [position, dimensions]);

  const style = {
    height: heightState,
    width: widthState,
    top: yState,
    left: xState,
  };

  let outline_style = "";
  switch (active) {
    case 0:
      outline_style = "bg-blue-400 menu-button-outline  border-blue-300";
      break;
    case 1:
      outline_style = "card-outline";
      break;
  }

  return (
    /*  <div className='cursor bg-blue-400 absolute z-10 border-8 border-blue-300 transition-all ' style={style}>
            </div> */

    <div className={`cursor transition-all z-10 absolute ${outline_style}`} style={style}></div>
  );
};

export default Cursor;
