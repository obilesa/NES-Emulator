import SpeechBubble from "../assets/speech-bubble.png";

interface MenuLabelProps {
  label: string;
  hidden?: boolean;
}

const MenuLabel: React.FC<MenuLabelProps> = ({ label, hidden }) => {
  return (
    <div className={` h-[7.6vh] w-[13vw] absolute flex  justify-center z-50 ${hidden ? "hidden" : ""} `} style={{ transform: "translateY(90%) " }}>
      <img src={SpeechBubble} className="h-full  pixelated " alt=""></img>
      <div className="content-center flex items-center justify-center text-[2.5vh]	 absolute h-[67%] w-[12.22vw] top-[2.1vh] left-[3%]">{label}</div>
    </div>
  );
};

export default MenuLabel;
