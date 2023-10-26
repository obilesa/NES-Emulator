import { useEffect, useRef, useContext } from "react";
import { CursorContext, Dimensions, Position } from "../contexts/CursorContext";
import { calculateNewValue } from "@testing-library/user-event/dist/utils";

interface CardProps {
  image: string;
  nmrOfPlayers: number;
  saves: {
    [key: number]: string;
  };
  position: number;
  selectedCard: number;
  slidingCard: boolean;
  active: boolean;
}

const Card: React.FC<CardProps> = ({ image, nmrOfPlayers, saves, selectedCard, position, active, slidingCard }) => {
  const myRef = useRef<SVGPathElement>(null);
  const { setPosition, setDimensions } = useContext(CursorContext);

  useEffect(() => {
    if (selectedCard === position && active) {
      let width = myRef.current?.getBoundingClientRect().width ?? 0;
      let height = myRef.current?.getBoundingClientRect().height ?? 0;
      let border = width / 39.625;

      const cardDimensions = {
        width: width + border * 2,
        height: height + border * 2,
      } as Dimensions;

      let x = myRef.current?.getBoundingClientRect().x ?? 0;
      let y = myRef.current?.getBoundingClientRect().y ?? 0;

      const cardPosition = { x: x - border, y: y - border } as Position;

      setPosition(cardPosition);
      setDimensions(cardDimensions);
    }
  }, [selectedCard, active]);

  return (
    <svg
      id="Layer_1"
      data-name="Layer 1"
      xmlns="http://www.w3.org/2000/svg"
      viewBox="0 0 82 90"
      className={`card absolute ${slidingCard ? "transition-none" : "transition-left"}`}
      data-position={position}
    >
      <style>
        {`
          .border{
            fill: ${selectedCard === position ? "#0073ef" : "#bcbcbc"};
          }
          `}
      </style>
      <defs>
        <clipPath id="poster">
          <path d="m77,2H3v1h-1v67h76V3h-1v-1Z" />
        </clipPath>
      </defs>
      <path d="m74,74v-1h-17v1h-1v9h1v1h17v-1h1v-9h-1Z" style={{ fill: `${selectedCard === position ? "#0073ef" : "#757575"}` }} />
      <path
        d="m77,70H2v15h1v1h74v-1h1v-15h-1Zm-63,9v3h-1v1h-4v-1h-1v-4h1v-1h4v1h1v1Zm8,0v3h-1v1h-4v-1h-1v-4h1v-1h4v1h1v1Zm2,2v-3h1v-1h4v1h1v4h-1v1h-4v-1h-1v-1Zm8,1v-4h1v-1h4v1h1v4h-1v1h-4v-1h-1Zm24,1v-9h1v-1h17v1h1v9h-1v1h-17v-1h-1Z"
        style={{ fill: `${selectedCard === position ? "#57a7ff" : "#e6e6e6"}` }}
      />
      <path d="m81,6v-2h-1v83h-1v1H2v1h1v1h78v-1h1V6h-1Z" />
      <g>
        <path d="m35,82h2v-4h-4v4h2Z" style={{ fill: `${selectedCard === position ? "#57a7ff" : "#e6e6e6"}` }} />
        <path d="m33,82v1h4v-1h-4Z" style={{ fill: `${selectedCard === position ? "#0073ef" : "#bcbcbc"}` }} />
        <path d="m37,82h1v-4h-1v4Z" style={{ fill: `${selectedCard === position ? "#0073ef" : "#bcbcbc"}` }} />
        <path d="m32,78v4h1v-4h-1Z" style={{ fill: `${selectedCard === position ? "#0073ef" : "#bcbcbc"}` }} />
        <path d="m37,77h-4v1h4v-1Z" style={{ fill: `${selectedCard === position ? "#0073ef" : "#bcbcbc"}` }} />
      </g>
      <g>
        <path d="m25,80v2h4v-4h-4v2Z" style={{ fill: `${selectedCard === position ? "#57a7ff" : "#e6e6e6"}` }} />
        <path d="m25,82v1h4v-1h-4Z" style={{ fill: `${selectedCard === position ? "#0073ef" : "#bcbcbc"}` }} />
        <path d="m29,82h1v-4h-1v4Z" style={{ fill: `${selectedCard === position ? "#0073ef" : "#bcbcbc"}` }} />
        <path d="m24,78v4h1v-4h-1Z" style={{ fill: `${selectedCard === position ? "#0073ef" : "#bcbcbc"}` }} />
        <path d="m29,77h-4v1h4v-1Z" style={{ fill: `${selectedCard === position ? "#0073ef" : "#bcbcbc"}` }} />
      </g>
      <g>
        <path d="m21,80v-2h-4v4h4v-2Z" style={{ fill: `${selectedCard === position ? "#57a7ff" : "#e6e6e6"}` }} />
        <path d="m17,82v1h4v-1h-4Z" style={{ fill: `${selectedCard === position ? "#0073ef" : "#bcbcbc"}` }} />
        <path d="m21,82h1v-4h-1v4Z" style={{ fill: `${selectedCard === position ? "#0073ef" : "#bcbcbc"}` }} />
        <path d="m16,78v4h1v-4h-1Z" style={{ fill: `${selectedCard === position ? "#0073ef" : "#bcbcbc"}` }} />
        <path d="m21,77h-4v1h4v-1Z" style={{ fill: `${selectedCard === position ? "#0073ef" : "#bcbcbc"}` }} />
      </g>
      <g>
        <path d="m13,80v-2h-4v4h4v-2Z" style={{ fill: `${selectedCard === position ? "#57a7ff" : "#e6e6e6"}` }} />
        <path d="m9,82v1h4v-1h-4Z" style={{ fill: `${selectedCard === position ? "#0073ef" : "#bcbcbc"}` }} />
        <path d="m13,82h1v-4h-1v4Z" style={{ fill: `${selectedCard === position ? "#0073ef" : "#bcbcbc"}` }} />
        <path d="m8,78v4h1v-4h-1Z" style={{ fill: `${selectedCard === position ? "#0073ef" : "#bcbcbc"}` }} />
        <path d="m13,77h-4v1h4v-1Z" style={{ fill: `${selectedCard === position ? "#0073ef" : "#bcbcbc"}` }} />
      </g>
      <path
        ref={myRef}
        d="m79,1V0H1v1H0v86h1v1h78v-1h1V1h-1Zm-1,3v81h-1v1H3v-1h-1V3h1v-1h74v1h1v1Z"
        style={{ fill: `${selectedCard === position ? "#0073ef" : "#bcbcbc"}` }}
      />
      <path d="m77,2H3v1h-1v67h76V3h-1v-1Z" style={{ fill: `${selectedCard === position ? "#3fbfff" : "#fff"}` }}></path>
      <image height="100%" preserveAspectRatio="xMidYMid meet" width="100%" clipPath="url(#poster)" href={image} />
      <g>
        <g>
          <path d="m63,81v-6h-2v1h-1v1h1v4h-2v1h6v-1h-2Z" style={{ fill: "#fff" }} />
          <path d="m72,76v-1h-6v7h2v-2h4v-1h1v-3h-1Zm-2,3h-2v-3h3v3h-1Z" style={{ fill: "#fff" }} />
        </g>
        <g>
          <path d="m69,76h-1v3h3v-3h-2Z" style={{ fill: `${selectedCard === position ? "#0073ef" : "#757575"}` }} />
          <path
            d="m74,74v-1h-17v1h-1v9h1v1h17v-1h1v-9h-1Zm-9,8h-6v-1h2v-4h-1v-1h1v-1h2v6h2v1Zm8-5v2h-1v1h-4v2h-2v-7h6v1h1v1Z"
            style={{ fill: `${selectedCard === position ? "#0073ef" : "#757575"}` }}
          />
        </g>
      </g>
      <g>
        <path d="m72,76v-1h-6v7h2v-2h4v-1h1v-3h-1Zm-2,3h-2v-3h3v3h-1Z" style={{ fill: "#fff" }} />
        <path d="m64,76v-1h-5v1h-1v1h2v-1h3v1h-1v1h-2v1h-1v1h-1v2h7v-1h-4v-1h2v-1h1v-1h1v-2h-1Z" style={{ fill: "#fff" }} />
        <g>
          <path d="m69,76h-1v3h3v-3h-2Z" style={{ fill: `${selectedCard === position ? "#0073ef" : "#757575"}` }} />
          <path
            d="m74,74v-1h-17v1h-1v9h1v1h17v-1h1v-9h-1Zm-1,3v2h-1v1h-4v2h-2v-7h6v1h1v1Zm-15,4v-1h1v-1h1v-1h2v-1h1v-1h-3v1h-2v-1h1v-1h5v1h1v2h-1v1h-1v1h-2v1h4v1h-7v-1Z"
            style={{ fill: `${selectedCard === position ? "#0073ef" : "#757575"}` }}
          />
        </g>
      </g>
    </svg>
  );
};

export default Card;
