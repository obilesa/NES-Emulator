import React, { createContext, useState } from "react";

export type Position = {
  x: number;
  y: number;
};

export type Dimensions = {
  width: number;
  height: number;
};

export const CursorContext = createContext({
  position: { x: 0, y: 0 },
  dimensions: { width: 0, height: 0 },
  setPosition: (position: Position) => {},
  setDimensions: (dimensions: Dimensions) => {},
});

type CursorContextProviderProps = {
  children: React.ReactNode;
};

export default function CursorContextProvider({ children }: CursorContextProviderProps) {
  const [position, setPosition] = useState<Position>({ x: 0, y: 0 });
  const [dimensions, setDimensions] = useState<Dimensions>({ width: 0, height: 0 });

  return (
    <CursorContext.Provider
      value={{
        position: position,
        dimensions: dimensions,
        setPosition: (position) => {
          setPosition(() => position);
        },
        setDimensions: (dimensions) => {
          setDimensions(() => dimensions);
        },
      }}
    >
      {children}
    </CursorContext.Provider>
  );
}
