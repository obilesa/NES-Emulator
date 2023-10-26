import React from "react";
import nintendoLogo from "../icons/nintendo-logo.png";
interface FooterProps {}

const horizontal_lines = Array.from({ length: 3 }, (_, i) => (
  <div className="w-[8vw] h-[0.8vh] bg-gray-800 z-20 " key={i}>
    {" "}
  </div>
));

const grid_lines = Array.from({ length: 10 }, (_, i) => (
  <div className="w-[0.5vw] bg-black h-[2.4vh] " key={i}>
    {" "}
  </div>
));

const Footer: React.FC<FooterProps> = () => {
  return (
    <footer className="relative">
      <div className="h-[0.4vh] bg-white relative"></div>
      <div className="bg-gray-400 h-[9vh] relative">
        <div className="ml-[5.5vw] absolute z-10 top-[0.8vh] " style={{ height: "6vh", width: "32vw" }}>
          <img src={nintendoLogo} className="h-full pixelated object-contain " alt=""></img>
        </div>
      </div>
      <div className="bg-gray-800 h-[4vh] w-full">
        <div className="absolute ml-[7vw] bottom-0   h-min flex flex-row gap-[0.5vw]">{grid_lines}</div>
        <div className="bg-black w-[9vw] h-full absolute right-[6vw] bottom-0 flex-col  items-center">
          <div className="absolute bottom-0 h-min w-full p-1 z-10 pb-0 flex gap-[0.4vh] flex-col items-center ">{horizontal_lines}</div>
          <div className="h-[0.4vh] bg-gray-800 top-0 z-20 w-full"></div>
        </div>
      </div>
    </footer>
  );
};

export default Footer;
