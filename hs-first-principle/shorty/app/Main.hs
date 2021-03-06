{-# LANGUAGE OverloadedStrings#-}
module Main where

import Control.Monad (replicateM)
import Control.Monad.IO.Class (liftIO)
import qualified Data.ByteString.Char8 as BC
import Data.Text.Encoding (decodeUtf8, encodeUtf8)
import qualified Data.Text.Lazy as TL
import qualified Database.Redis as R
import Network.URI (URI, parseURI)
import qualified System.Random as SR
import Web.Scotty


-- the parameter be passed to randomElement
alphaNum :: String
alphaNum = ['A'..'Z'] ++ ['0'..'9']

-- take optional character list, and return a random char.
-- randomRIO use the global random number
randomElement :: String -> IO Char
randomElement xs = do
    let maxIndex :: Int
        maxIndex = length xs -1
    randomDigit <- SR.randomRIO (0, maxIndex) :: IO Int
    return (xs !! randomDigit)

-- replicateM replicate over the IO monad structure.
shortyGen :: IO String
shortyGen = replicateM 7 (randomElement alphaNum)

-- set the key and value for redis.
saveURI :: R.Connection
        -> BC.ByteString
        -> BC.ByteString
        -> IO (Either R.Reply R.Status)
saveURI conn shortURI uri =
    R.runRedis conn $ R.set shortURI uri

-- ask for uri based on the shorty version in redis.
getURI :: R.Connection
       -> BC.ByteString
       -> IO (Either R.Reply (Maybe BC.ByteString))
getURI conn shortURI = R.runRedis conn $ R.get shortURI

-- shorty uri get demonstrated on the browser.
linkShorty :: String -> String
linkShorty shorty =
    concat [ "<a href=\""
           , shorty
           , "\"> Copy and paste your short URL </a>"
           ]

shortyCreated :: Show a => a -> String -> TL.Text
shortyCreated resp shawty =
    TL.concat [ TL.pack (show resp)
              , " shorty is: ", TL.pack (linkShorty shawty)
              ]

shortyAintUri :: TL.Text -> TL.Text
shortyAintUri uri =
    TL.concat [ uri
              , " wasn't a url, did you forget http:///?"
              ]

shortyFound :: TL.Text -> TL.Text
shortyFound tbs =
    TL.concat [ "<a herf=\"", tbs, "\">", tbs, "</a>"]

-- ScottyM () indicates this monad return nothing into the system.
app :: R.Connection
    -> ScottyM ()
app rConn = do
    get "/" $ do
        uri <- param "uri"
        let parseUri :: Maybe URI
            parseUri = parseURI (TL.unpack uri)
        case parseUri of
            Just _ -> do
                shawty <- liftIO shortyGen
                let shorty = BC.pack shawty
                    uri' = encodeUtf8 (TL.toStrict uri)
                resp <- liftIO (saveURI rConn shorty uri')
                html (shortyCreated resp shawty)
            Nothing -> text (shortyAintUri uri)
    get "/:short" $ do
        short <- param "short"
        uri <- liftIO (getURI rConn short)
        case uri of
            Left reply -> text (TL.pack (show reply))
            Right mbBS -> case mbBS of
                Nothing -> text "uri not found"
                Just bs -> html (shortyFound tbs)
                    where tbs :: TL.Text
                          tbs = TL.fromStrict (decodeUtf8 bs)
main :: IO ()
main = do
    rConn <- R.connect R.defaultConnectInfo
    scotty 3000 (app rConn)

