interface GameTitleProps {
  title: string;
  selectedCard: number;
}

const GameTitle: React.FC<GameTitleProps> = ({ title, selectedCard }) => {
  return <div className={`title absolute `}>{title}</div>;
};

export default GameTitle;
