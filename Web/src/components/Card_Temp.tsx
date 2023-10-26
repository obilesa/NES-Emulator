import { useContext, useEffect, useRef, useState } from "react";
import { CursorContext, Dimensions, Position } from "../contexts/CursorContext";

interface CardTempProps {
    position: number;
    selectedCard: number;
    slidingCard: boolean;
    active: boolean;
}

const CardTemp:React.FC<CardTempProps> = ({position, selectedCard,slidingCard,active}) => {
    const myRef = useRef<HTMLDivElement>(null)
    const {setPosition, setDimensions} = useContext(CursorContext)
    useEffect(() => {
        if(selectedCard == position && active){
            const cardDimensions = {
                width: myRef.current?.getBoundingClientRect().width,
                height: myRef.current?.getBoundingClientRect().height,
              } as Dimensions;

              const cardPosition = { x: myRef.current?.getBoundingClientRect().x, y: myRef.current?.getBoundingClientRect().y } as Position;
              
            setPosition(cardPosition)
            setDimensions(cardDimensions);
        }
    }, [selectedCard,active])


    return (
        <div ref={myRef} className={`card aspect-square  absolute  bg-black ${slidingCard ? "transition-none" : "transition-left"}`} data-position={position} >
            </div>
    )
}

export default CardTemp;