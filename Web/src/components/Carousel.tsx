import Card from "./Card";
import { KeyDownEvent } from "../App";
import data from "../api/cards.json";
import { useState, useEffect, useRef } from "react";
import { get } from "http";

interface CarouselProps {
  keyDown: KeyDownEvent;
  active: boolean;
  setSelectedGame: React.Dispatch<React.SetStateAction<{ title: string; index: number }>>;
}

// assigns an index to each card based on the selected card, these indexes are used to calculate the css
function getCardPosition(numberOfCards: number, selectedCard: number, index: number) {
  let mid = Math.floor(numberOfCards / 2);
  if (mid + selectedCard - numberOfCards > index) {
    return numberOfCards - selectedCard + index + 1;
  } else if (mid + selectedCard < index + 1) {
    return (selectedCard + (numberOfCards - index - 1)) * -1;
  } else {
    return index + 1 - selectedCard;
  }
}

function getAbsoluteCardPosition(middleCard: number, selectedCard: number, numberOfCards: number) {
  if ((middleCard + selectedCard) % numberOfCards < 0) {
    return numberOfCards + ((middleCard + selectedCard) % numberOfCards);
  } else {
    return (middleCard + selectedCard) % numberOfCards;
  }
}

function calculateCardPosition(numberOfCards: number, cardWidth: number, index: number) {
  let gapWidth = (100 - cardWidth * (numberOfCards - 3)) / (numberOfCards - 3);
  let start = 0 - (cardWidth * 1.5 + gapWidth);
  return start + index * (cardWidth + gapWidth);
}

// calculate the maximum number of cards that can be displayed on the screen
function getNumberOfCards(cardsLoaded: number) {
  let width = window.innerWidth;
  let height = window.innerHeight;
  let cardHeight = height * 0.37;
  let cardWidth = (cardHeight / 23) * 21.7;

  let numberOfCards = cardsLoaded;
  if (cardsLoaded < Math.floor(width / cardWidth + width * 0.7)) {
    numberOfCards = Math.floor(width / cardWidth) + 3;
  }
  let cards = Array(numberOfCards).fill(0) as number[];
  for (let i = 0; i < numberOfCards; i++) {
    cards[i] = getCardPosition(numberOfCards, Math.ceil(numberOfCards / 2), i);
  }
  return cards;
}

const Carousel: React.FC<CarouselProps> = ({ keyDown, active, setSelectedGame }) => {
  const [middleCard, setMiddleCard] = useState(4);
  const [selectedCard, setSelectedCard] = useState(0);
  const [cards, setCards] = useState(data.cards);
  const [slidingCard, setSlidingCard] = useState(0);
  const [visibleCards, setVisibleCards] = useState<number[]>(getNumberOfCards(cards.length));
  const carouselRef = useRef<HTMLDivElement>(null);

  // handles the keyboard inputs
  useEffect(() => {
    if (!active) return;
    let cardCount = carouselRef.current?.childElementCount || 0;
    if (keyDown.key === "ArrowRight") {
      // Sets index for game title
      setSelectedGame({
        title: cards[getAbsoluteCardPosition(middleCard, selectedCard, cards.length)].name,
        index: getAbsoluteCardPosition(middleCard, selectedCard, cards.length),
      });
      // This makes the carousel move
      // Why was this 3? I changed it to 2 now it fixed the bug when scrolling small carousels on small resolutions
      // Changed it back to 3, but the behavior is not consistent
      if (selectedCard === visibleCards[visibleCards.length - 3]) {
        setSlidingCard(Math.floor(cardCount / 2));
        setMiddleCard((middleCard + 1) % cardCount);
        return;
      }
      setSelectedCard(selectedCard + 1);
    } else if (keyDown.key === "ArrowLeft") {
      //TODO: find out the reason for the -2
      // This sets the index and title for the title component
      setSelectedGame({
        title: cards[getAbsoluteCardPosition(middleCard, selectedCard - 2, cards.length)].name,
        index: getAbsoluteCardPosition(middleCard, selectedCard - 2, cards.length),
      });
      // This Handles the movement of the carousel
      if (selectedCard === visibleCards[2]) {
        setMiddleCard((middleCard - 1 + cardCount) % cardCount);
        setSlidingCard(Math.floor(cardCount / 2) + (cardCount - 1) * -1);
        return;
      }
      setSelectedCard(selectedCard - 1);
    }
  }, [keyDown.timeStamp]);

  // calculates the css for the cards
  useEffect(() => {
    let styleSheet = document.styleSheets[1];
    let cardWidth = ((((window.innerHeight * 0.37) / 23) * 21.7) / window.innerWidth) * 100;
    styleSheet.insertRule(`
      .card{
        width: ${cardWidth}vw;
      }
    `);
    styleSheet.insertRule(`
      .card[data-position]{
        display: none;
      `);
    styleSheet.insertRule("card[data-position] {display: none");
    styleSheet.insertRule(".card[data-position]:first-child {display: block; opacity: 0;}");

    // are there enough card to fill the carousel?
    if (cards.length !== visibleCards.length) {
      // YES
      for (let i = 0; i < visibleCards.length; i++) {
        //The gap between the cards is calc here
        styleSheet.insertRule(`
        .card[data-position="${visibleCards[i]}"]{
          left: ${calculateCardPosition(visibleCards.length, cardWidth, i)}vw;
          display: block !important;
          opacity: 100% !important;
        }
      `);
      }
    } else {
      // NO
      for (let i = 0; i < visibleCards.length; i++) {
        console.log(cardWidth);
        styleSheet.insertRule(`
        .card[data-position="${visibleCards[i]}"]{
          left: ${(0 + cardWidth) / 2 + i * 0.7 + cardWidth * i}vw;
          display: block !important;
          opacity: 100% !important;
        }
      `);
      }
    }
    setSelectedGame({
      title: cards[getAbsoluteCardPosition(middleCard, selectedCard - 1, cards.length)].name,
      index: getAbsoluteCardPosition(middleCard, selectedCard, cards.length),
    });
  }, []);

  let carouselCards = [] as JSX.Element[];
  // Edge case when there are not enough cards to for the carousel to work normally (the cards sliding from the side are missing) so we duplicate the cards
  if (visibleCards.length >= cards.length && cards.length >= visibleCards.length - 3) {
    for (let i = 0; i < cards.length * 2; i++) {
      let position = getCardPosition(cards.length * 2, middleCard, i);
      carouselCards.push(
        <Card
          position={position}
          key={i}
          selectedCard={selectedCard}
          slidingCard={slidingCard === position}
          active={active}
          image={cards[i % cards.length].image}
          nmrOfPlayers={0}
          saves={{}}
        />
      );
    }
  } else {
    carouselCards = cards.map((card, index) => {
      let position = getCardPosition(cards.length, middleCard, index);
      return (
        <Card
          position={position}
          key={index}
          selectedCard={selectedCard}
          slidingCard={slidingCard === position}
          active={active}
          image={card.image}
          nmrOfPlayers={0}
          saves={{}}
        />
      );
    });
  }
  return (
    <div ref={carouselRef} className={`carousel w-full h-[40vh] relative overflow-hidden flex items-center`}>
      {carouselCards}
    </div>
  );
};

export default Carousel;
