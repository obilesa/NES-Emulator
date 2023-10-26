import Carousel from "./Carousel";
import { KeyDownEvent } from "../App";
import Card from "./Card";
import GameTitle from "./GameTitle";
import { useState } from "react";

interface BodyProps {
  keyDown: KeyDownEvent;
  active: boolean;
}

const Body: React.FC<BodyProps> = ({ keyDown, active }) => {
  const [selectedGame, setSelectedGame] = useState({
    title: "",
    index: 0,
  });
  return (
    <div className="relative h-full flex-grow flex">
      <GameTitle selectedCard={selectedGame.index} title={selectedGame.title} />
      <Carousel keyDown={keyDown} active={active} setSelectedGame={setSelectedGame} />
    </div>
  );
};

export default Body;
